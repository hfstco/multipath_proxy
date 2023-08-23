//
// Created by Matthias Hofstätter on 20.08.23.
//

#include <cstring>
#include "UnsortedBacklog.h"

#include "Chunk.h"
#include "../exception/Exception.h"
#include "TotalBacklog.h"

namespace backlog {

    UnsortedBacklog::UnsortedBacklog() :    Backlog() {

    }

    void UnsortedBacklog::reset() {
        std::lock_guard lock(_mutex);

        Chunk *chunk;
        while (!_chunks.empty()) {
            chunk = _chunks.front();
            _chunks.pop();
            total_backlog -= chunk->header.size;
            delete chunk;
        }

        _size = 0;
        _offset = 0;
    }

    void UnsortedBacklog::insert(Chunk *chunk) {
        std::lock_guard lock(_mutex);

        _chunks.push(chunk);

        _size += chunk->header.size;
        total_backlog += chunk->header.size;
    }

    Chunk *UnsortedBacklog::next(uint64_t max) {
        std::lock_guard lock(_mutex);

        max = std::min(max, _size.load());

        Chunk *chunk = nullptr;

        // close packet
        if (max == 0 && !_chunks.empty()) {
            chunk = _chunks.front();

            _chunks.pop();
        }

        if (max > 0) {
            chunk = new Chunk;
            chunk->data = static_cast<unsigned char *>(malloc(max));
            chunk->header.offset = _offset;
            chunk->header.size = max;

            uint64_t written = 0;
            auto current = _chunks.front();
            while (!_chunks.empty() && written != max) {
                uint64_t offset = (_offset - current->header.offset);
                uint64_t toRead = std::min((max - written), current->header.size - offset);
                memcpy(chunk->data + written, current->data + offset, toRead);
                _offset += toRead;
                written += toRead;

                // remove chunk if read completely
                if (offset + toRead == current->header.size) {
                    _chunks.pop();
                    delete current;
                }

                current = _chunks.front();
            }

            _size -= written;
            total_backlog -= written;
        }

        return chunk;
    }

    UnsortedBacklog::~UnsortedBacklog() {
        reset();
    }
} // backlog