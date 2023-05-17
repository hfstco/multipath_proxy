//
// Created by Matthias Hofstätter on 25.04.23.
//

#include <glog/logging.h>

#include "HeartBeatPacket.h"

#include "HeartBeatHeader.h"

namespace packet {

    HeartBeatPacket::HeartBeatPacket() : Packet(0) {
        packet::HeartBeatHeader heartBeatHeader = packet::HeartBeatHeader();
        memcpy(this->header(), &heartBeatHeader, sizeof(packet::HeartBeatHeader));
    }

    HeartBeatPacket *HeartBeatPacket::make() {
        return new HeartBeatPacket();
    }

    packet::HeartBeatHeader *HeartBeatPacket::header() {
        return reinterpret_cast<packet::HeartBeatHeader *>(Packet::header());
    }

    uint16_t HeartBeatPacket::size() {
        return sizeof(packet::HeartBeatHeader);
    }

    std::string HeartBeatPacket::ToString() {
        return "HBP[header=" + header()->ToString() + ",Size=" + std::to_string(size()) + "]";
    }

    HeartBeatPacket::~HeartBeatPacket() {
        LOG(INFO) << "del " + ToString();
    }

} // packet
