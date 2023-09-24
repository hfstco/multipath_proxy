//
// Created by Matthias Hofstätter on 30.05.23.
//

#ifndef MULTIPATH_PROXY_BLOCKINGFLOWPACKETQUEUE_H
#define MULTIPATH_PROXY_BLOCKINGFLOWPACKETQUEUE_H

#include <deque>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <cassert>
#include <glog/logging.h>

#include "../packet/FlowPacket.h"
#include "../packet/FlowHeader.h"
#include "IFlowPacketQueue.h"

namespace collections {

    class BlockingFlowPacketQueue : IFlowPacketQueue, std::deque<packet::FlowPacket *> {
    public:
        BlockingFlowPacketQueue() : nextAvailable_(false), size_(0), currentId_(0), byteSize_(0) {}

        const uint64_t byteSize() const {
            return byteSize_.load();
        }

        uint64_t currentId() const {
            return currentId_.load();
        }

        // insert FlowPacket into dequeue
        void Insert(packet::FlowPacket *flowPacket) {
            std::lock_guard lock(mutex_);

            // insert packet into dequeue
            if (deque_.empty()) { // if deque is empty -> push front
                deque_.push_front(flowPacket);
            } else if (flowPacket->header()->id() < deque_.front()->header()->id()) { // if packet id is smaller than first packet in queue -> push front
                deque_.push_front(flowPacket);
            } else { // sorted insert packet from back
                // loop backwards
                for (auto it = deque_.crbegin(); it != deque_.crend(); ++it) {
                    // check if id of the packets is smaller than current packet
                    if ((*it)->header()->id() < flowPacket->header()->id()) {
                        // insert packet
                        deque_.insert(it.base(), flowPacket);
                        break;
                    }
                }
            }

            // increase size of dequeue
            size_ += 1;
            // update backlog
            byteSize_ += flowPacket->header()->size();

            // notify waiting threads that next packet is available
            if (deque_.front()->header()->id() == currentId_.load()) {
                nextAvailable_ = true;
                conditionVariable_.notify_one();
            }
        }

        // pop next packet
        packet::FlowPacket *Pop() {
            std::unique_lock lock(mutex_);
            // cv, waiting for next packet available
            conditionVariable_.wait(lock, [this] { return nextAvailable_; });

            // pop packet
            packet::FlowPacket *flowPacket = deque_.front();
            assert(flowPacket->header()->id() == currentId_.load());
            deque_.pop_front();

            // increase next packet counter
            currentId_.fetch_add(1);
            // decrease size of dequeue
            size_ -= 1;
            // update backlog
            byteSize_.fetch_sub(flowPacket->header()->size());

            if (deque_.empty() || deque_.front()->header()->id() != currentId_.load()) {
                // next packet isn't available
                nextAvailable_ = false;
            } else {
                // next packet is available -> do nothing
                assert(deque_.front()->header()->id() == currentId_.load());
            }

            lock.unlock();

            return flowPacket;
        }

        bool Empty() {
            std::unique_lock lock(mutex_);

            return deque_.empty();
        }

        void Clear() {
            std::unique_lock lock(mutex_);

            size_ = 0;
            deque_.clear();
            nextAvailable_ = false;
        }

        std::string ToString() {
            return "BlockingFlowPacketQueue[size=" + std::to_string(size_) + ", currentId=" + std::to_string(currentId_) + ", byteSize=" + std::to_string(byteSize_) + "]";
        }

    private:
        mutable std::mutex mutex_;
        bool nextAvailable_ = false;
        std::condition_variable conditionVariable_;

        std::deque<packet::FlowPacket *> deque_;

        uint64_t size_;
        std::atomic<uint64_t> currentId_;
        std::atomic<uint64_t> byteSize_;
    };

} // collections

#endif //MULTIPATH_PROXY_BLOCKINGFLOWPACKETQUEUE_H
