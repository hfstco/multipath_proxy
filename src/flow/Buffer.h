//
// Created by Matthias Hofstätter on 27.08.23.
//

#ifndef MPP_BUFFER_H
#define MPP_BUFFER_H

#include <cstdint>

namespace flow {

    template<uint64_t SIZE = 1523>
    struct Buffer {
        uint8_t data[SIZE] {};
        uint64_t size = 0;
    };

} // flow

#endif //MPP_BUFFER_H
