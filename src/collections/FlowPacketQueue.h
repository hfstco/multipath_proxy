//
// Created by Matthias Hofstätter on 16.04.23.
//
//      push(packet_id)
//      pop(packet_id)
//
//      push(1), push(3), push(4), push(2), pop() -> 1, pop() -> 2, pop() -> 3, pop() -> 4
//      push(1), push(3), pop() -> 1, pop() -> nullptr, push(2), pop() -> 2, pop() -> 3

#ifndef MULTIPATH_PROXY_FLOWPACKETQUEUE_H
#define MULTIPATH_PROXY_FLOWPACKETQUEUE_H

#include <mutex>
#include <vector>

namespace packet {
    class FlowPacket;
}

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
