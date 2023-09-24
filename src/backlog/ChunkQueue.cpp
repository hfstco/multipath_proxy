//
// Created by Matthias Hofstätter on 20.08.23.
//

#include <cstring>
#include "ChunkQueue.h"

#include "Chunk.h"
#include "../exception/Exception.h"
#include "TotalBacklog.h"

namespace backlog {

    ChunkQueue::ChunkQueue() {

    }

    void ChunkQueue::clear() {
        std::lock_guard lock(_mutex);

        _clear();
    }

    void ChunkQueue::push(Chunk *chunk) {
        std::lock_guard lock(_mutex);

        _push(chunk);
    }

    Chunk *ChunkQueue::pop() {
        std::lock_guard lock(_mutex);

        return _pop();
    }

    ChunkQueue::~ChunkQueue() {
        clear();
    }

    void ChunkQueue::_clear() {
        while (!_chunks.empty()) {
            delete _chunks.front();
            _chunks.pop();
        }
    }

    void ChunkQueue::_push(Chunk *chunk) {
        _chunks.push(chunk);
    }

    Chunk *ChunkQueue::_pop() {
        Chunk *chunk = nullptr;

        if (!_chunks.empty()) {
            chunk = _chunks.front();
            _chunks.pop();
        }

        return chunk;
    }
} // backlog