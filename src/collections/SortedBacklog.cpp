//
// Created by Matthias Hofstätter on 30.05.23.
//

#include "SortedBacklog.h"

#include "../packet/PicoPacket.h"

namespace collections {
    //void SortedBacklog::push(packet::FlowPacket *flowPacket) {
    void SortedBacklog::push(packet::PicoPacket *pico_packet) {
        std::lock_guard lock(_mutex);

        //DLOG(INFO) << ToString() << ".Insert(" << flowPacket->ToString() << ")";

        if (_packets.empty()) { // if deque is empty
            //LOG(INFO) << "PUSH EMPTY";
            //_packets.push_front(flowPacket);
            _packets.push_front(pico_packet);
        //} else if (flowPacket->header()->id() < _packets.front()->header()->id()) {
        } else if (pico_packet->id < _packets.front()->id) {
            //LOG(INFO) << "PUSH FRONT";
            //_packets.push_front(flowPacket);
            _packets.push_front(pico_packet);
        } else {
            for (auto it = _packets.crbegin(); it != _packets.crend(); ++it) {
                //if ((*it)->header()->id() < flowPacket->header()->id()) {
                if ((*it)->id < pico_packet->id) {
                    //LOG(INFO) << "PUSH " << flowPacket->header()->id() << " BEFORE " << (*it)->header()->id();
                    //_packets.insert(it.base(), flowPacket);
                    _packets.insert(it.base(), pico_packet);
                    break;
                }
            }
        }

        //_size += flowPacket->header()->size();
        _size += pico_packet->size;

        //if (_packets.front()->header()->id() == _current_id.load()) {
        if (_packets.front()->id == _current_id.load()) {
            _next_available = true;
            _conditional_variable.notify_one();
        }
    }

    //packet::FlowPacket *SortedBacklog::pop() {
    packet::PicoPacket *SortedBacklog::pop() {
        std::unique_lock lock(_mutex);
        _conditional_variable.wait(lock, [this] { return _next_available; });

        //DLOG(INFO) << ToString() << ".Pop()";

        //packet::FlowPacket *flowPacket = _packets.front();
        packet::PicoPacket *pico_packet = _packets.front();
        //assert(flowPacket->header()->id() == _current_id.load());
        assert(pico_packet->id == _current_id.load());
        _packets.pop_front();

        _current_id++;
        //_size -= flowPacket->header()->size();
        _size -= pico_packet->size;

        //if (_packets.empty() || _packets.front()->header()->id() != _current_id.load()) {
        if (_packets.empty() || _packets.front()->id != _current_id.load()) {
            _next_available = false;
        } else {
            //assert(_packets.front()->header()->id() == _current_id.load());
            assert(_packets.front()->id == _current_id.load());
        }

        lock.unlock();

        //return flowPacket;
        return pico_packet;
    }

    bool SortedBacklog::empty() {
        std::unique_lock lock(_mutex);

        return _packets.empty();
    }

    void SortedBacklog::reset() {
        std::unique_lock lock(_mutex);

        while (!_packets.empty()) {
            delete _packets.front();
            _packets.pop_front();
        }

        _size = 0;
        _next_available = false;
    }

    std::string SortedBacklog::to_string() {
        return "SortedBacklog[current_id=" + std::to_string(_current_id) + ", size=" + std::to_string(_size) + "]";
    }

    SortedBacklog::~SortedBacklog() {
        reset();
    }
} // collections