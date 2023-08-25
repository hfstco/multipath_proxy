//
// Created by Matthias Hofstätter on 23.08.23.
//

#include "UnsortedBacklog.h"

#include "../packet/FlowHeader.h"
#include "../packet/PicoPacket.h"

namespace collections {

    //void UnsortedBacklog::push(packet::FlowPacket *flowPacket) {
    void UnsortedBacklog::push(packet::PicoPacket *pico_packet) {
        std::lock_guard lock(_mutex);

        //_packets.push(flowPacket);
        _packets.push(pico_packet);
        //_size += flowPacket->header()->size();
        _size += pico_packet->size;

        _next_available = true;
        _conditional_variable.notify_one();
    }

    //packet::FlowPacket *UnsortedBacklog::pop() {
    packet::PicoPacket *UnsortedBacklog::pop() {
        std::unique_lock lock(_mutex);
        _conditional_variable.wait(lock, [this] { return _next_available; });

        //packet::FlowPacket *flow_packet = _packets.front();
        packet::PicoPacket *pico_packet = _packets.front();
        _packets.pop();

        //_size -= flow_packet->header()->size();
        _size -= pico_packet->size;
        _current_id++;

        if (!_packets.empty()) {
            _next_available = true;
            _conditional_variable.notify_one();
        } else {
            _next_available = false;
        }

        return pico_packet;
    }

    bool UnsortedBacklog::empty() {
        return _size == 0;
    }

    void UnsortedBacklog::reset() {
        std::lock_guard lock(_mutex);

        while(!_packets.empty()) {
            delete _packets.front();
            _packets.pop();
        }

        _size = 0;
        _current_id = 0;
    }

    std::string UnsortedBacklog::to_string() {
        return "UnsortedBacklog[current_id=" + std::to_string(_current_id) + ", size=" + std::to_string(_size) + "]";
    }

    UnsortedBacklog::~UnsortedBacklog() {
        reset();
    }
} // collections