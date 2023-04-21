//
// Created by Matthias Hofstätter on 16.04.23.
//

#include "FlowPacketQueue.h"

namespace collections {

    bool FlowPacketQueue::empty() {
        const std::lock_guard lock(this->mutex_);

        return std::vector<packet::FlowPacket *>::empty();
    }

    size_t FlowPacketQueue::size() {
        const std::lock_guard lock(this->mutex_);

        return std::vector<packet::FlowPacket *>::size();
    }

    void FlowPacketQueue::push(packet::FlowPacket *packet) {
        const std::lock_guard lock(this->mutex_);

        std::vector<packet::FlowPacket *>::push_back(packet);
    }

    packet::FlowPacket *FlowPacketQueue::pop() {
        const std::lock_guard lock(this->mutex_);

        if(std::vector<packet::FlowPacket *>::empty())
            return nullptr;

        // TODO optimize search algo, will not return if not found
        /*FlowPacket *packet = reinterpret_cast<FlowPacket *>(PacketQueue::pop());
        while (packet->header()->id() != currentId_) {
            PacketQueue::push(packet);
            packet = reinterpret_cast<FlowPacket *>(PacketQueue::pop());
        }*/

        // iterate through whole vector to find package TODO
        for(auto flowPacket = this->begin(); flowPacket < this->end(); flowPacket++) {
            if((*flowPacket)->header()->id() == currentId_) {
                currentId_ += 1;
                packet::FlowPacket *p = (*flowPacket);
                this->erase(flowPacket);
                return p;
            }
        }

        // package with currentId not available yet
        return nullptr;
    }

    uint32_t FlowPacketQueue::currentId() {
        const std::lock_guard lock(this->mutex_);

        return currentId_;
    }

} // collections