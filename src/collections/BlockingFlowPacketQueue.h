//
// Created by Matthias Hofstätter on 30.05.23.
//

#ifndef MULTIPATH_PROXY_BLOCKINGFLOWPACKETQUEUE_H
#define MULTIPATH_PROXY_BLOCKINGFLOWPACKETQUEUE_H

#include <deque>
#include <atomic>
#include <condition_variable>
#include <assert.h>
#include <glog/logging.h>

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

        const uint64_t currentId() const {
            return currentId_.load(std::memory_order_relaxed);
        }

        void Insert(packet::FlowPacket *flowPacket) {
            std::lock_guard lock(mutex_);

            //DLOG(INFO) << ToString() << ".Insert(" << flowPacket->ToString() << ")";

            if (deque_.empty()) { // if deque is empty
                //LOG(INFO) << "PUSH EMPTY";
                deque_.push_front(flowPacket);
            } else if (flowPacket->header()->id() < deque_.front()->header()->id()) {
                //LOG(INFO) << "PUSH FRONT";
                deque_.push_front(flowPacket);
            } else {
                for (auto it = deque_.crbegin(); it != deque_.crend(); ++it) {
                    if ((*it)->header()->id() < flowPacket->header()->id()) {
                        //LOG(INFO) << "PUSH " << flowPacket->header()->id() << " BEFORE " << (*it)->header()->id();
                        deque_.insert(it.base(), flowPacket);
                        break;
                    }
                }
            }

            size_ += 1;
            byteSize_ += flowPacket->header()->size();

            if (deque_.front()->header()->id() == currentId_.load()) {
                nextAvailable_ = true;
                conditionVariable_.notify_one();
            }
        }

        packet::FlowPacket *Pop() {
            std::unique_lock lock(mutex_);
            conditionVariable_.wait(lock, [this] { return nextAvailable_; });

            //DLOG(INFO) << ToString() << ".Pop()";

            packet::FlowPacket *flowPacket = std::move(deque_.front());
            assert(flowPacket->header()->id() == currentId_.load(std::memory_order_relaxed));
            deque_.pop_front();

            currentId_.fetch_add(1);
            size_ -= 1;
            byteSize_.fetch_sub(flowPacket->header()->size());

            if (deque_.empty() || deque_.front()->header()->id() != currentId_.load(std::memory_order_relaxed)) {
                nextAvailable_ = false;
            } else {
                assert(deque_.front()->header()->id() == currentId_.load(std::memory_order_relaxed));
                conditionVariable_.notify_one();
            }

            lock.unlock();

            return flowPacket;
        }

        bool Empty() {
            std::unique_lock lock(mutex_);

            return deque_.empty();
        }

        void Clear() {
            DLOG(INFO) << ToString() << ".Clear()";

            std::unique_lock lock(mutex_);

            size_ = 0;
            deque_.clear();
            nextAvailable_ = false;
        }

        std::string Print() {
            std::stringstream stringStream;
            for (auto it = deque_.cbegin(); it != deque_.cend(); ++it) {
                LOG(INFO) << (*it)->ToString();
            }

            return stringStream.str();
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
