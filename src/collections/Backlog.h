//
// Created by Matthias Hofstätter on 23.08.23.
//

#include <string>
#include <cstdint>
#include <atomic>
#include <condition_variable>

#ifndef MPP_FLOWPACKETQUEUE_H
#define MPP_FLOWPACKETQUEUE_H

namespace packet {
    class FlowPacket;
    class PicoPacket;
}

namespace collections {

    class Backlog {
    public:
        uint64_t size() {
            std::lock_guard lock(_mutex);

            return _size;
        }

        uint64_t currentId() {
            std::lock_guard lock(_mutex);

            return _current_id;
        }

        //virtual void push(packet::FlowPacket *flowPacket) = 0;
        virtual void push(packet::PicoPacket *pico_packet) = 0;
        //virtual packet::FlowPacket *pop() = 0;
        virtual packet::PicoPacket *pop() = 0;
        virtual bool empty() = 0;
        virtual void reset() = 0;

        virtual std::string to_string() {
            return "Backlog[size=" + std::to_string(_size) + ", current_id=" + std::to_string(_current_id) + "]";
        }

        virtual ~Backlog() = default;

    protected:
        mutable std::mutex _mutex;
        bool _next_available = false;
        std::condition_variable _conditional_variable;

        std::atomic<uint64_t> _size = 0;
        std::atomic<uint64_t> _current_id = 0;
    };
}

#endif //MPP_FLOWPACKETQUEUE_H
