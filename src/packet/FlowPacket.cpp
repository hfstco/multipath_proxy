//
// Created by Matthias Hofstätter on 25.04.23.
//

#include <glog/logging.h>
#include <cassert>

#include "FlowPacket.h"

#include "FlowHeader.h"
#include "../exception/Exception.h"

namespace packet {

    FlowPacket::FlowPacket(FlowHeader flowHeader, uint16_t size) : Packet(sizeof(FlowHeader) + size) {
        flowHeader.size(size);
        memcpy(header(), &flowHeader, sizeof(FlowHeader));
    }

    FlowPacket::FlowPacket(packet::FlowHeader flowHeader) : FlowPacket(flowHeader, 0) {}

    FlowPacket *FlowPacket::make(packet::FlowHeader header, uint16_t size) {
        return new FlowPacket(header, size);
    }

    FlowPacket *FlowPacket::make(packet::FlowHeader header) {
        return new FlowPacket(header, 0);
    }

    FlowHeader *FlowPacket::header() {
        return (FlowHeader *)packet::Packet::header();
    }

    unsigned char *FlowPacket::data() {
        return (unsigned char *)Packet::header() + sizeof(FlowHeader);
    }

    uint16_t FlowPacket::size() {
        return Packet::size();
    }

    void FlowPacket::Resize(uint16_t size) {
        try {
            Packet::Resize(sizeof(FlowHeader) + size);
            this->header()->size(size);
        } catch (BufferReallocError bufferReallocError) {
            throw bufferReallocError;
        }
    }

    std::string FlowPacket::ToString() {
        return "FlowPacket[header=" + header()->ToString() + ",size=" + std::to_string(size()) + /*", " + std::to_string(header()->checksum()) +*/ "]"; // + std::string(data(), data() + header()->size());
    }

    FlowPacket::~FlowPacket() {
        //LOG(INFO) << "del " + ToString();
    }

    /*uint64_t FlowPacket::CalcChecksum(FlowPacket *flowPacket) {
        size_t result = 0;
        const size_t prime = 31;
        for (size_t i = 0; i < flowPacket->header()->size(); ++i) {
            result = flowPacket->data()[i] + (result * prime);
        }
        return result;
    }*/

} // packet