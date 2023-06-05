//
// Created by Matthias Hofstätter on 30.05.23.
//

#ifndef MULTIPATH_PROXY_BLOCKINGFLOWPACKETQUEUE_H
#define MULTIPATH_PROXY_BLOCKINGFLOWPACKETQUEUE_H

#include <deque>
#include <atomic>
#include <condition_variable>
#include <assert.h>

#include "../packet/FlowPacket.h"
#include "../packet/FlowHeader.h"
#include "IFlowPacketQueue.h"

/*namespace packet {
    class FlowPacket;
}*/

namespace collections {

    class BlockingFlowPacketQueue : IFlowPacketQueue, std::deque<packet::FlowPacket *> {
    public:
        BlockingFlowPacketQueue() : nextAvailable_(false), size_(0), currentId_(0), byteSize_(0) {}

        const uint64_t byteSize() const {
            return byteSize_.load(std::memory_order_relaxed);
        }

        void Insert(packet::FlowPacket *flowPacket) {
            std::lock_guard lock(mutex_);

            if (deque_.empty() || flowPacket->header()->id() == currentId_) { // if deque is empty or packet is currentId_
                deque_.push_front(flowPacket);
            } else {
                for (auto it = deque_.crbegin(); it != deque_.crend(); ++it) {
                    if ((*it)->header()->id() < flowPacket->header()->id()) {
                        deque_.insert(it.base(), flowPacket);

                        break;
                    }
                }
            }

            size_ += 1;
            byteSize_ += flowPacket->header()->size();

            if (flowPacket->header()->id() == currentId_) {
                nextAvailable_ = true;
                conditionVariable_.notify_one();
            }
        }

        packet::FlowPacket *Pop() {
            std::unique_lock lock(mutex_);
            conditionVariable_.wait(lock, [this] { return nextAvailable_; });

            packet::FlowPacket *flowPacket = std::move(deque_.front());
            assert(flowPacket->header()->id() == currentId_);
            deque_.pop_front();

            currentId_ += 1;
            size_ -= 1;
            byteSize_ -= flowPacket->header()->size();

            if (deque_.empty() || deque_.front()->header()->id() != currentId_) {
                nextAvailable_ = false;
            } else {
                assert(deque_.front()->header()->id() == currentId_);
                conditionVariable_.notify_one();
            }

            lock.unlock();

            return flowPacket;
        }

    private:
        std::mutex mutex_;
        bool nextAvailable_ = false;
        std::condition_variable conditionVariable_;

        std::deque<packet::FlowPacket *> deque_;

        uint64_t size_;
        uint64_t currentId_;
        std::atomic<uint64_t> byteSize_;
    };

} // collections

#endif //MULTIPATH_PROXY_BLOCKINGFLOWPACKETQUEUE_H
