//
// Created by Matthias Hofstätter on 03.08.23.
//

#ifndef MPP_DATABUFFER_H
#define MPP_DATABUFFER_H

#include <cstdio>

namespace net {

    class DataBuffer {
    public:

    private:
        unsigned char *_data;
        size_t _size;
    };

} // net

#endif //MPP_DATABUFFER_H
