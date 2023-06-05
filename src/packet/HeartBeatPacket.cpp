//
// Created by Matthias Hofstätter on 25.04.23.
//

#include <glog/logging.h>

#include "HeartBeatPacket.h"

#include "HeartBeatHeader.h"

namespace packet {

    HeartBeatPacket::HeartBeatPacket() : Packet(sizeof(HeartBeatHeader)) {
        HeartBeatHeader heartBeatHeader = HeartBeatHeader();
        memcpy(header(), &heartBeatHeader, sizeof(HeartBeatHeader));
    }

    HeartBeatPacket *HeartBeatPacket::make() {
        return new HeartBeatPacket();
    }

    packet::HeartBeatHeader *HeartBeatPacket::header() {
        return reinterpret_cast<HeartBeatHeader *>(Packet::header());
    }

    uint16_t HeartBeatPacket::size() {
        return sizeof(HeartBeatHeader);
    }

    std::string HeartBeatPacket::ToString() {
        return "HeartBeatPacket[header=" + header()->ToString() + ",Size=" + std::to_string(size()) + "]";
    }

    HeartBeatPacket::~HeartBeatPacket() {
        VLOG(4) << ToString() << ".~HeartBeatPacket()";
    }

} // packet
