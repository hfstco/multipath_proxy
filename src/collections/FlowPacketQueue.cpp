//
// Created by Matthias Hofstätter on 16.04.23.
//

#include <glog/logging.h>

#include "FlowPacketQueue.h"

#include "../packet/FlowPacket.h"
#include "../packet/FlowHeader.h"

namespace collections {

    bool FlowPacketQueue::empty() {
        const std::lock_guard lock(this->mutex_);

        return std::vector<packet::FlowPacket *>::empty();
    }

    size_t FlowPacketQueue::size() {
        const std::lock_guard lock(this->mutex_);

        return std::vector<packet::FlowPacket *>::size();
    }

    void FlowPacketQueue::push(packet::FlowPacket *pFlowPacket) {
        const std::lock_guard lock(this->mutex_);

        std::vector<packet::FlowPacket *>::push_back(pFlowPacket);

        LOG(INFO) << "push(" << pFlowPacket->ToString() << ")";
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
        for(auto itFlowPacket = this->begin(); itFlowPacket < this->end(); itFlowPacket++) {
            if((*itFlowPacket)->header()->id() == currentId_) {
                currentId_ += 1;
                packet::FlowPacket *pFlowPacket = (*itFlowPacket);
                this->erase(itFlowPacket);
                LOG(INFO) << "pop() -> " + pFlowPacket->ToString();
                return pFlowPacket;
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