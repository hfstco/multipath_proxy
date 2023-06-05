//
// Created by Matthias Hofstätter on 25.04.23.
//

#include <glog/logging.h>
#include <cassert>

#include "Packet.h"

#include "Buffer.h"
#include "Header.h"

namespace packet {

    Packet::Packet(uint16_t size) : Buffer(size) {
        assert(size >= sizeof(Header));
    }

    packet::Header *Packet::header() {
        return (packet::Header *)Buffer::data();
    }

    uint16_t Packet::size() {
        return Buffer::size();
    }

    std::string Packet::ToString() {
        return Buffer::ToString();
    }

}
