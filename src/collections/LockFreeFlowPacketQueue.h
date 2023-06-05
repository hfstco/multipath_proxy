//
// Created by Matthias Hofstätter on 16.04.23.
//
//      push(packet_id)
//      pop(packet_id)
//
//      push(1), push(3), push(4), push(2), pop() -> 1, pop() -> 2, pop() -> 3, pop() -> 4
//      push(1), push(3), pop() -> 1, pop() -> nullptr, push(2), pop() -> 2, pop() -> 3

#ifndef MULTIPATH_PROXY_LOCKFREEFLOWPACKETQUEUE_H
#define MULTIPATH_PROXY_LOCKFREEFLOWPACKETQUEUE_H

#include <mutex>
#include <vector>
#include <atomic>

namespace packet {
    struct FlowPacket;
}

namespace collections {

    struct LockFreeFlowPacketQueueNode {
        packet::FlowPacket *flowPacket;
        std::atomic<LockFreeFlowPacketQueueNode *> next;

        LockFreeFlowPacketQueueNode(packet::FlowPacket *flowPacket) : flowPacket(flowPacket), next(nullptr) {}
    };

    class LockFreeFlowPacketQueue {
    public:
        LockFreeFlowPacketQueue();

        size_t size() const;
        uint64_t transCount() const;
        uint32_t currentId() const;
        uint64_t byteSize() const;

        bool Empty();
        void Clear();

        void Insert(packet::FlowPacket *flowPacket);
        packet::FlowPacket *Pop();

        std::string ToString();

        virtual ~LockFreeFlowPacketQueue();

    private:
        std::atomic<LockFreeFlowPacketQueueNode *> head_;
        std::atomic<LockFreeFlowPacketQueueNode *> tailHint_;
        std::atomic<size_t> size_;
        std::atomic<uint64_t> transCount_;
        std::atomic<uint32_t> currentId_;
        std::atomic<uint64_t> byteSize_;
    };

} // collections

#endif //MULTIPATH_PROXY_LOCKFREEFLOWPACKETQUEUE_H
