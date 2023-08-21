//
// Created by Matthias Hofstätter on 18.08.23.
//

#include "ChunkHeader.h"

namespace packet {
    std::string ChunkHeader::ToString() {
        return "ChunkHeader[offset=" + std::to_string(offset) + ", size=" + std::to_string(size) + "]";
    }
} // packet