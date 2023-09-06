//
// Created by Matthias Hofstätter on 02.09.23.
//

#ifndef MPP_CHUNK_H
#define MPP_CHUNK_H

#include <cstdint>

namespace backlog {

    struct Chunk {
        Chunk() = delete;
        explicit Chunk(uint64_t length);

        uint64_t offset();
        void offset(uint64_t offset);
        uint64_t length();
        void length(uint64_t length);
        uint8_t *bytes();

        uint8_t *data();
        uint64_t size();

    private:
        uint8_t *_data;
    };

} // backlog

#endif //MPP_CHUNK_H
