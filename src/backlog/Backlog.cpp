//
// Created by Matthias Hofstätter on 06.09.23.
//

#include "Backlog.h"
#include "TotalBacklog.h"
#include "Chunk.h"

namespace backlog {

    Backlog::Backlog() :    ChunkQueue(),
                            _size(0) {

    }

    uint64_t Backlog::size() {
        return _size;
    }

    void Backlog::clear() {
        std::lock_guard lock(_mutex);

        _clear();

        TOTAL_BACKLOG -= _size;
        _size = 0;
    }

    void Backlog::push(Chunk *chunk) {
        std::lock_guard lock(_mutex);

        _push(chunk);

        TOTAL_BACKLOG += chunk->length();
        _size += chunk->length();
    }

    Chunk *Backlog::pop() {
        std::lock_guard lock(_mutex);

        Chunk *chunk = _pop();

        if (chunk != nullptr) {
            TOTAL_BACKLOG -= chunk->length();
            _size -= chunk->length();
        }

        return chunk;
    }

    Backlog::~Backlog() {
        clear();
    }
} // backlog