//
// Created by Matthias Hofstätter on 16.04.23.
//

#ifndef MULTIPATH_PROXY_FLOWPACKETQUEUE_H
#define MULTIPATH_PROXY_FLOWPACKETQUEUE_H

#include <mutex>
#include "../packet/FlowPacket.h"

namespace collections {

    class FlowPacketQueue : private std::vector<packet::FlowPacket *> {
    public:
        FlowPacketQueue() : std::vector<packet::FlowPacket *>(), currentId_(0) {
            std::vector<packet::FlowPacket *>().reserve(100);
        }

        uint32_t currentId();

        bool empty();
        size_t size();

        void push(packet::FlowPacket *packet);
        packet::FlowPacket *pop();

    private:
        std::mutex mutex_;
        uint32_t currentId_;
    };

} // collections

#endif //MULTIPATH_PROXY_FLOWPACKETQUEUE_H
