//
// Created by Matthias Hofstätter on 03.08.23.
//

#include "SortedBacklog.h"

#include <cstring>
#include <glog/logging.h>

#include "Chunk.h"

namespace backlog {

    SortedBacklog::SortedBacklog() :    Backlog(),
                                        _total(0) {
    }

    void SortedBacklog::reset() {
        std::lock_guard lock(_mutex);

        for (auto &it : _chunks) {
            delete it.second;
        }

        _chunks.clear();
        _size = 0;
        _total = 0;
        _offset = 0;
    }

    void SortedBacklog::insert(backlog::Chunk *chunk) {
        std::lock_guard lock(_mutex);

        // insert
        _chunks.insert({chunk->header.offset, chunk});

        // update size
        _total += chunk->header.size;

        // update available
        _size = 0;
        auto current = _chunks.begin();
        if (current->second->header.offset <= _offset) { // && _rx_offset.load() < current->second->offset + current->second->size
            // first chunk
            _size += current->second->header.size;
            uint64_t lastOffset = current->second->header.offset + current->second->header.size;
            current++;
            while (current != _chunks.end() && lastOffset == current->second->header.offset) {
                _size += current->second->header.size;
                lastOffset = current->second->header.offset + current->second->header.size;
                current++;
            }
            // offset
            _size -= _offset - _chunks.begin()->second->header.offset;
        }
    }

    backlog::Chunk *SortedBacklog::next(uint64_t max) {
        std::lock_guard lock(_mutex);

        max = std::min(_size.load(), max);
        /*if (_size < max) {
            get = _size;
        }*/

        Chunk *chunk = nullptr;

        // close packet
        if (max == 0 && !_chunks.empty()) {
            chunk = new Chunk;
            chunk->header.offset = _offset;
            chunk->header.size = 0;
        }

        if (max > 0) {
            chunk = new Chunk;
            chunk->data = static_cast<unsigned char *>(malloc(max));
            chunk->header.offset = _offset;
            chunk->header.size = max;

            uint64_t written = 0;
            auto current = _chunks.begin();
            while (current != _chunks.end() && written != max) {
                uint64_t offset = (_offset - current->second->header.offset);
                uint64_t toRead = std::min((max - written), current->second->header.size - offset);
                memcpy(chunk->data + written, current->second->data + offset, toRead);
                _offset += toRead;
                written += toRead;

                // remove chunk if read completely
                if (offset + toRead == current->second->header.size) {
                    delete current->second;
                    current++;
                    _chunks.erase(std::prev(current));
                }
            }

            _size -= written;
            _total -= written;
        }

        return chunk;
    }

    SortedBacklog::~SortedBacklog() {
        reset();
    }
} // backlog
