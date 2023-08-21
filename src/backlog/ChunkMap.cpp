//
// Created by Matthias Hofstätter on 03.08.23.
//

#include "ChunkMap.h"

#include <cstring>
#include <glog/logging.h>

namespace backlog {

    ChunkMap::ChunkMap() :  _size(0),
                            _offset(0),
                            _available(0) {
    }

    const uint64_t ChunkMap::size() {
        return _size.load();
    }

    const uint64_t ChunkMap::offset() {
        return _offset.load();
    }

    const uint64_t ChunkMap::available() {
        return _available.load();
    }

    bool ChunkMap::Empty() {
        return _size.load() == 0;
    }

    void ChunkMap::Reset() {
        std::lock_guard lock(_mutex);

        for (auto &it : _chunks) {
            delete it.second;
        }

        _chunks.clear();
        _size.store(0);
        _offset.store(0);
    }

    void ChunkMap::Insert(backlog::Chunk *chunk) {
        std::lock_guard lock(_mutex);

        // insert
        _chunks.insert({chunk->offset, chunk});

        // update size
        _size.fetch_add(chunk->size);

        // update available
        _available.store(0);
        auto current = _chunks.begin();
        if (current->second->offset <= _offset.load()) { // && _offset.load() < current->second->offset + current->second->size
            // first chunk
            _available.fetch_add(current->second->size);
            uint64_t lastOffset = current->second->offset + current->second->size;
            current++;
            while (current != _chunks.end() && lastOffset == current->second->offset) {
                _available.fetch_add(current->second->size);
                lastOffset = current->second->offset + current->second->size;
                current++;
            }
            // offset
            _available.fetch_sub(_offset.load() - _chunks.begin()->second->offset);
        }
    }

    backlog::Chunk *ChunkMap::Next(uint64_t max) {
        std::lock_guard lock(_mutex);

        uint64_t get = max;
        if (_available.load() < max) {
            get = _available.load();
        }

        Chunk *chunk = nullptr;
        if (get > 0) {
            chunk = new Chunk;
            chunk->data = static_cast<unsigned char *>(malloc(max));
            chunk->offset = _offset.load();
            chunk->size = get;

            uint64_t written = 0;
            auto current = _chunks.begin();
            while (current != _chunks.end() && written != get) {
                uint64_t offset = (_offset.load() - current->second->offset);
                uint64_t toRead = std::min((get - written), current->second->size - offset);
                memcpy(chunk->data + written, current->second->data + offset, toRead);
                _offset.fetch_add(toRead);
                written += toRead;

                // remove chunk if read completely
                if (offset + toRead == current->second->size) {
                    delete current->second;
                    current++;
                    _chunks.erase(std::prev(current));
                }
            }

            _available.fetch_sub(written);
            _size.fetch_sub(written);
        }

        return chunk;
    }

    ChunkMap::~ChunkMap() {
        Reset();
    }
} // backlog
