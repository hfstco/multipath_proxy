//
// Created by Matthias Hofstätter on 14.04.23.
//

#ifndef MULTIPATH_PROXY_HEARTBEATPACKET_H
#define MULTIPATH_PROXY_HEARTBEATPACKET_H

#include "Packet.h"

namespace packet {

    class HeartBeatHeader;

    struct HeartBeatPacket : public Packet {
    public:
        static HeartBeatPacket *make();

        HeartBeatHeader *header();

        uint16_t size();

        std::string ToString();

        ~HeartBeatPacket() override;

    private:
        HeartBeatPacket();
    };

} // packet

#endif //MULTIPATH_PROXY_HEARTBEATPACKET_H
