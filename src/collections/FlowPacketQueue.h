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
#include <atomic>

namespace packet {
    class FlowPacket;
}

namespace collections {

    class FlowPacketQueue : private std::vector<packet::FlowPacket *> {
    public:
        FlowPacketQueue();

        uint32_t CurrentId();
        uint64_t byteSize();

        bool Empty();
        size_t Size();

        void Push(packet::FlowPacket *flowPacket);
        packet::FlowPacket *Pop();

        void Clear();

        virtual ~FlowPacketQueue();

    private:
        std::mutex mutex_;
        uint32_t currentId_;

        uint64_t byteSize_;
    };

} // collections

#endif //MULTIPATH_PROXY_FLOWPACKETQUEUE_H
