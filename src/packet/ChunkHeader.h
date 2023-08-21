//
// Created by Matthias Hofstätter on 18.08.23.
//

#ifndef MPP_CHUNKHEADER_H
#define MPP_CHUNKHEADER_H

#include <string>


namespace packet {

    struct __attribute__((packed)) ChunkHeader {
        unsigned char *data;
        uint64_t offset;
        uint64_t size;

        std::string ToString();
    };

} // packet

#endif //MPP_CHUNKHEADER_H
