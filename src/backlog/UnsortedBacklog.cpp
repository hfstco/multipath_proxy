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

        while (!_chunks.empty()) {
            delete _chunks.front();
            _chunks.pop();
        }
        //_chunks.erase(_chunks.begin(), _chunks.end());

        _size = 0;
        _offset = 0;

        total_backlog -= _size;
    }

    void UnsortedBacklog::insert(Chunk *chunk) {
        std::lock_guard lock(_mutex);

        _offset += chunk->length();
        _size += chunk->length();

        total_backlog += chunk->length();

        _chunks.push(chunk);
    }

    Chunk *UnsortedBacklog::next() {
        std::lock_guard lock(_mutex);

        Chunk *chunk = nullptr;

        if (!_chunks.empty()) {
            chunk = _chunks.front();
            _chunks.pop();

            _size -= chunk->length();

            total_backlog -= chunk->length();
        }

        return chunk;
    }

    UnsortedBacklog::~UnsortedBacklog() {
        reset();
    }
} // backlog