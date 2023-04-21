//
// Created by Matthias Hofstätter on 14.04.23.
//

#ifndef MULTIPATH_PROXY_HEARTBEATPACKET_H
#define MULTIPATH_PROXY_HEARTBEATPACKET_H

#include "Packet.h"
#include "header/HeartBeatHeader.h"

namespace packet {

    struct HeartBeatPacket : public Packet {
    public:
        static HeartBeatPacket *make() {
            return new HeartBeatPacket();
        }

        packet::header::HeartBeatHeader *header() {
            return reinterpret_cast<packet::header::HeartBeatHeader *>(Packet::header());
        }

    private:
        HeartBeatPacket() : Packet(sizeof(packet::header::HeartBeatHeader)) {
            packet::header::HeartBeatHeader heartBeatHeader = packet::header::HeartBeatHeader();
            memcpy(this->header(), &heartBeatHeader, sizeof(packet::header::HeartBeatHeader));
        }
    };

} // packet

#endif //MULTIPATH_PROXY_HEARTBEATPACKET_H
