//
// Created by Matthias Hofstätter on 23.08.23.
//

#ifndef MPP_UNSORTEDBACKLOG_H
#define MPP_UNSORTEDBACKLOG_H

#include "Backlog.h"

#include <queue>

#include "../packet/FlowPacket.h"

namespace collections {

    class UnsortedBacklog : public Backlog {
    public:
        //void push(packet::FlowPacket *flowPacket) override;
        void push(packet::PicoPacket *pico_packet) override;
        //packet::FlowPacket *pop() override;
        packet::PicoPacket *pop() override;

        bool empty() override;
        void reset() override;

        std::string to_string() override;

        ~UnsortedBacklog() override;

    private:
        //std::queue<packet::FlowPacket *> _packets;
        std::queue<packet::PicoPacket *> _packets;
    };

} // collections

#endif //MPP_UNSORTEDBACKLOG_H
