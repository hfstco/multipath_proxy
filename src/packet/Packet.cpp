//
// Created by Matthias Hofstätter on 25.04.23.
//

#include <glog/logging.h>

#include "Packet.h"

#include "Buffer.h"
#include "Header.h"

namespace packet {
    Packet::Packet(size_t size) : packet::Buffer(size + sizeof(packet::Header)) {}
    Packet::Packet() : Packet(0) {}


    packet::Header *Packet::header() {
        return reinterpret_cast<packet::Header *>(packet::Buffer::data());
    };

    unsigned char *Packet::data() {
        return packet::Buffer::data() + sizeof(packet::Header);
    }

    uint16_t Packet::size() {
        return packet::Buffer::size() - sizeof(packet::Header);
    }

    void Packet::resize(size_t size) {
        packet::Buffer::resize(sizeof(packet::Header) + size);
    }

    std::string Packet::ToString() {
        return "P[header=" + header()->ToString() + ",size=" + std::to_string(size()) + "]";
    }

    Packet::~Packet() {
        LOG(INFO) << "del " + ToString();
    }
}
