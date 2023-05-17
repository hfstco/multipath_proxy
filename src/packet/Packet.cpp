//
// Created by Matthias Hofstätter on 25.04.23.
//

#include <glog/logging.h>

#include "Packet.h"

#include "Buffer.h"
#include "Header.h"

namespace packet {
    Packet::Packet(uint16_t size) : packet::Buffer(size + sizeof(packet::Header)) {}


    packet::Header *Packet::header() {
        return reinterpret_cast<packet::Header *>(packet::Buffer::data());
    };

    unsigned char *Packet::data() {
        return packet::Buffer::data() + sizeof(packet::Header);
    }

    uint16_t Packet::size() {
        return packet::Buffer::size();
    }

    void Packet::Resize(uint16_t size) {
        packet::Buffer::Resize(sizeof(packet::Header) + size);
    }

    std::string Packet::ToString() {
        return "Packet[header=" + header()->ToString() + ",Size=" + std::to_string(size()) + "]";
    }

    Packet::~Packet() {
        LOG(INFO) << "del " + ToString();
    }
}
