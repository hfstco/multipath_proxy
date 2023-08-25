//
// Created by Matthias Hofstätter on 16.04.23.
// https://beneschtech.medium.com/a-lock-free-sorted-list-my-implementation-da153bb19077
// https://github.com/beneschtech/LockFreeSortedList/blob/main/LockFreeSortedList.h
//

#include <glog/logging.h>

#include "LockFreeFlowPacketQueue.h"

#include "../packet/FlowPacket.h"
#include "../packet/FlowHeader.h"
#include "../exception/Exception.h"

namespace collections {
    LockFreeFlowPacketQueue::LockFreeFlowPacketQueue() : head_(nullptr), tailHint_(nullptr), size_(0), currentId_(0), byteSize_(0) {
        DLOG(INFO) << "FlowPacketQueue() * ";
    }

    size_t LockFreeFlowPacketQueue::size() const {
        return size_.load();
    }

    uint64_t LockFreeFlowPacketQueue::transCount() const {
        return transCount_.load();
    }

    uint32_t LockFreeFlowPacketQueue::currentId() const {
        return currentId_.load();
    }

    uint64_t LockFreeFlowPacketQueue::byteSize() const {
        return byteSize_.load();
    }

    bool LockFreeFlowPacketQueue::Empty() {
        return size_.load() == 0;
    }

    void LockFreeFlowPacketQueue::Insert(packet::FlowPacket *flowPacket) {
        transCount_.fetch_add(1);
        LockFreeFlowPacketQueueNode *ptr = head_.load();
        if (!ptr) { // empty list
            LockFreeFlowPacketQueueNode *newNode = new LockFreeFlowPacketQueueNode(flowPacket);
            head_.store(newNode);
            tailHint_.store(newNode); // add hint if last element changed
            size_.fetch_add(1,std::memory_order_relaxed);
            byteSize_.fetch_add(flowPacket->header()->size(),std::memory_order_relaxed); // TODO or flowPacket->size()

            //DLOG(INFO) << ToString() << ".Insert(" + flowPacket->ToString() + ")";

            return; // true;
        } else if (ptr->flowPacket->header()->id() < flowPacket->header()->id()) { // add last packet
            LockFreeFlowPacketQueueNode *newNode = new LockFreeFlowPacketQueueNode(flowPacket);
            newNode->next = ptr;
            if (!head_.compare_exchange_strong(ptr, newNode)) { // eventually it will work
                delete newNode;
                return Insert(flowPacket);
            }
            size_.fetch_add(1,std::memory_order_relaxed);
            byteSize_.fetch_add(flowPacket->header()->size(),std::memory_order_relaxed); // TODO or flowPacket->size()

            //DLOG(INFO) << ToString() << ".Insert(" + flowPacket->ToString() + ")";

            return;
            // TODO set head_?
        }

        // insert packet into correct position
        bool success = false;
        while (!success) {
            // find correct position
            LockFreeFlowPacketQueueNode *node = nullptr;
            while (ptr) {
                if (ptr->flowPacket->header()->id() == flowPacket->header()->id()) { // id already exists
                    IdAlreadyExistsException idAlreadyExistsException = IdAlreadyExistsException("");

                    //DLOG(INFO) << ToString() << ".Insert(flowPacket=" << flowPacket->ToString() << ") ! " << idAlreadyExistsException.ToString();

                    throw idAlreadyExistsException;
                }
                node = ptr->next;
                if (!node) { // next node is tail/nullptr
                    break;
                }
                if (node->flowPacket->header()->id() < flowPacket->header()->id()) {
                    break;
                }
                ptr = node;
            }

            // insert
            LockFreeFlowPacketQueueNode *newNode = new LockFreeFlowPacketQueueNode(flowPacket);
            newNode->next.store(node);
            if (!ptr->next.compare_exchange_strong(node, newNode)) {
                delete newNode;
                ptr = head_.load();
            } else {
                if (!node) { // add hint if last element changed
                    tailHint_.store(newNode);
                }
                success = true;
                size_.fetch_add(1);
                byteSize_.fetch_add(flowPacket->header()->size()); // TODO or flowPacket->size()

                //DLOG(INFO) << ToString() << ".Insert(" + flowPacket->ToString() + ")";
            }
        }
        return; // success;
    }

    packet::FlowPacket *LockFreeFlowPacketQueue::Pop() {
        transCount_.fetch_add(1);
        LockFreeFlowPacketQueueNode *ptr = head_.load();
        if(!head_) { // queue empty
            return nullptr;
        } else if (!ptr->next) { // head == tail
            if (ptr->flowPacket->header()->id() == currentId_.load()) { // flowPacket->id == currentid
                if (head_.compare_exchange_strong(ptr, nullptr)) {
                    size_.fetch_sub(1,std::memory_order_relaxed);
                    currentId_.fetch_add(1,std::memory_order_relaxed);
                    byteSize_.fetch_sub(ptr->flowPacket->header()->size(),std::memory_order_relaxed);  // TODO or flowPacket->size()
                    tailHint_ = nullptr;
                    packet::FlowPacket *flowPacket = ptr->flowPacket;
                    delete ptr;

                    //DLOG(INFO) << ToString() << ".Pop() -> " << flowPacket->ToString();

                    return flowPacket;
                } else {
                    return nullptr;
                }
            } else { // flowPacket->id != currentId
                return nullptr;
            }
        }
        // TODO use tailHint_
        /*FlowPacketQueueNode *tail = tailHint_.load();
        if(tail) {
            FlowPacketQueueNode *node = tail->next;
        }*/
        LockFreeFlowPacketQueueNode *node = nullptr, *last = nullptr;
        while(ptr) {
            node = ptr->next;
            if (!node) { // end of list
                return nullptr;
            }
            if (!node->next && node->flowPacket->header()->id() == currentId_.load()) { // reached tail
                if(!ptr->next.compare_exchange_strong(node, nullptr)) {
                    return nullptr;
                } else {
                    size_.fetch_sub(1,std::memory_order_relaxed);
                    currentId_.fetch_add(1,std::memory_order_relaxed);
                    byteSize_.fetch_sub(node->flowPacket->header()->size(),std::memory_order_relaxed);  // TODO or flowPacket->size()
                    if (last) {
                        tailHint_ = ptr; // TODO last of ptr
                    }
                    packet::FlowPacket *flowPacket = node->flowPacket;
                    delete node;

                    //DLOG(INFO) << ToString() << ".Pop() -> " << flowPacket->ToString();

                    return flowPacket;
                }
            }
            last = ptr;
            ptr = node;
        }

        return nullptr;
    }

    void LockFreeFlowPacketQueue::Clear() {
        LockFreeFlowPacketQueueNode *ptr = head_.load(), *node = nullptr;
        head_ = nullptr;
        while(ptr) {
            node = ptr->next;
            ptr->next = nullptr;
            delete ptr;
            ptr = node;
            if (!ptr) {
                ptr = head_.load();
                head_ = nullptr;
                tailHint_ = nullptr;
            }
        }
        size_.store(0);
        transCount_.store(0);
    }

    LockFreeFlowPacketQueue::~LockFreeFlowPacketQueue() {
        DLOG(INFO) << ToString() << ".~FlowPacketQueue()";

        Clear();
    }

    std::string LockFreeFlowPacketQueue::ToString() {
        return "FlowPacketQueue[size=" + std::to_string(size()) + ", currentId=" + std::to_string(currentId_) + ", size=" + std::to_string(byteSize_) + "]";
    }

} // collections