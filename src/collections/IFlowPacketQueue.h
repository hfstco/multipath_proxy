//
// Created by Matthias Hofstätter on 31.05.23.
//

#ifndef MULTIPATH_PROXY_IFLOWPACKETQUEUE_H
#define MULTIPATH_PROXY_IFLOWPACKETQUEUE_H

#include "../packet/FlowPacket.h"

namespace collections {

    class IFlowPacketQueue {
    public:
        virtual const uint64_t byteSize() const = 0;

        virtual void Insert(packet::FlowPacket *flowpacket) = 0;
        virtual packet::FlowPacket *Pop() = 0;
    };

} // collections

#endif //MULTIPATH_PROXY_IFLOWPACKETQUEUE_H
