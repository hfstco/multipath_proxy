//
// Created by Matthias Hofstätter on 02.09.23.
//

#include <cstdlib>
#include "Chunk.h"

namespace backlog {
    Chunk::Chunk(uint64_t length) : _data((uint8_t *) malloc(length  + (2 * sizeof(uint64_t)))) {
        *(uint64_t *) (_data + sizeof(uint64_t)) = length;
    }

    uint64_t Chunk::offset() {
        return *_data;
    }

    void Chunk::offset(uint64_t offset) {
        *_data = offset;
    }

    uint64_t Chunk::length() {
        return *(uint64_t *) (_data + sizeof(uint64_t));
    }

    void Chunk::length(uint64_t length) {
        *(uint64_t *) (_data + sizeof(uint64_t)) = length;
    }

    uint8_t *Chunk::bytes() {
        return _data + (2 * sizeof(uint64_t));
    }

    uint8_t *Chunk::data() {
        return _data;
    }

    uint64_t Chunk::size() {
        return *(_data + sizeof(uint64_t)) - (2 * sizeof(uint64_t));
    }
} // backlog