//
// Created by Matthias Hofstätter on 25.04.23.
//

#include <glog/logging.h>

#include "FlowPacket.h"

#include "FlowHeader.h"

namespace packet {

    FlowPacket::FlowPacket(packet::FlowHeader header, uint16_t size) : Packet((sizeof(packet::FlowHeader) - sizeof(packet::Header)) + size) {
        header.size(size);
        memcpy(this->header(), &header, sizeof(packet::FlowHeader));
    }

    FlowPacket::FlowPacket(packet::FlowHeader header) : FlowPacket(header, 0) {}

    FlowPacket::FlowPacket(packet::FlowHeader header, unsigned char *data, uint16_t size) : FlowPacket(header, size) {
        memcpy(this->data(), data, size);
    }

    FlowPacket *FlowPacket::make(packet::FlowHeader header, uint16_t size) {
        return new FlowPacket(header, size);
    }

    FlowPacket *FlowPacket::make(packet::FlowHeader header) {
        return new FlowPacket(header, 0);
    }

    FlowPacket *FlowPacket::make(packet::FlowHeader header, unsigned char *data, uint16_t size) {
        return new FlowPacket(header, data, size);
    }

    packet::FlowHeader *FlowPacket::header() {
        return reinterpret_cast<packet::FlowHeader *>(Packet::header());
    }

    unsigned char *FlowPacket::data() {
        return ((unsigned char *)packet::Packet::header()) + sizeof(packet::FlowHeader);
    }

    uint16_t FlowPacket::size() {
        return packet::Buffer::size();
    }

    void FlowPacket::Resize(uint16_t size) {
        packet::Packet::Resize((sizeof(packet::FlowHeader) - sizeof(packet::Header)) + size);
        this->header()->size(size);
    }

    std::string FlowPacket::ToString() {
        return "FlowPacket[header=" + header()->ToString() + ",Size=" + std::to_string(size()) + "]";
    }

    FlowPacket::~FlowPacket() {
        LOG(INFO) << "del " + ToString();
    }

} // packet