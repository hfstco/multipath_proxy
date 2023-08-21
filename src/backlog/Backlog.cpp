//
// Created by Matthias Hofstätter on 20.08.23.
//

#include "Backlog.h"
#include "../exception/Exception.h"

namespace backlog {
    void Backlog::Insert(Chunk *chunk) {
        try {
            TotalBacklog::_size.fetch_add(chunk->size);
            ChunkMap::Insert(chunk);
        } catch (Exception &e) {
            TotalBacklog::_size.fetch_sub(chunk->size);

            throw e;
        }
    }

    Chunk *Backlog::Next(uint64_t max) {
        Chunk *chunk = ChunkMap::Next(max);
        if (chunk) {
            TotalBacklog::_size.fetch_sub(chunk->size);
        }
        return chunk;
    }

    Backlog::~Backlog() = default;

    uint64_t TotalBacklog::size() {
        return TotalBacklog::_size.load();
    }
} // backlog