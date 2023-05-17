//
// Created by Matthias Hofstätter on 16.04.23.
//

#include <glog/logging.h>

#include "FlowPacketQueue.h"

#include "../packet/FlowPacket.h"
#include "../packet/FlowHeader.h"

namespace collections {
    FlowPacketQueue::FlowPacketQueue() : std::vector<packet::FlowPacket *>(), currentId_(0), byteSize_(0) {
        std::vector<packet::FlowPacket *>().reserve(1024);
    }

    bool FlowPacketQueue::Empty() {
        const std::lock_guard lock(mutex_);

        return std::vector<packet::FlowPacket *>::empty();
    }

    size_t FlowPacketQueue::Size() {
        const std::lock_guard lock(mutex_);

        return std::vector<packet::FlowPacket *>::size();
    }

    void FlowPacketQueue::Push(packet::FlowPacket *flowPacket) {
        const std::lock_guard lock(mutex_);

        byteSize_ += flowPacket->size();
        std::vector<packet::FlowPacket *>::push_back(flowPacket);
    }

    packet::FlowPacket *FlowPacketQueue::Pop() {
        const std::lock_guard lock(mutex_);

        if(std::vector<packet::FlowPacket *>::empty()) {
            return nullptr;
        }

        // iterate through whole vector to find package TODO
        for(auto flowPacketIterator = this->begin(); flowPacketIterator < this->end(); flowPacketIterator++) {
            if((*flowPacketIterator)->header()->id() == currentId_) {
                packet::FlowPacket *flowPacket = (*flowPacketIterator);
                this->erase(flowPacketIterator);

                currentId_ += 1;
                byteSize_ -= flowPacket->size();

                return flowPacket;
            }
        }

        // package with currentId not available yet
        return nullptr;
    }

    void FlowPacketQueue::Clear() {
        const std::lock_guard lock(mutex_);

        for(auto &flowPacket : (*this)) {
            delete flowPacket;
        }
    }

    uint32_t FlowPacketQueue::CurrentId() {
        const std::lock_guard lock(mutex_);

        return currentId_;
    }

    uint64_t FlowPacketQueue::byteSize() {
        const std::lock_guard lock(mutex_);

        return byteSize_;
    }

    FlowPacketQueue::~FlowPacketQueue() {
        LOG(INFO) << "~FlowPacketQueue()";

        Clear();
    }

} // collections