//
// Created by Matthias Hofstätter on 30.05.23.
//

#ifndef MULTIPATH_PROXY_BLOCKINGFLOWPACKETQUEUE_H
#define MULTIPATH_PROXY_BLOCKINGFLOWPACKETQUEUE_H

#include <deque>
#include <atomic>
#include <mutex>
#include <cassert>
#include <glog/logging.h>

#include "../packet/FlowPacket.h"
#include "../packet/FlowHeader.h"
#include "Backlog.h"

/*namespace packet {
    class FlowPacket;
}*/

namespace collections {

    class SortedBacklog : public Backlog {
    public:
        //void push(packet::FlowPacket *flowPacket) override;
        void push(packet::PicoPacket *pico_packet) override;
        //packet::FlowPacket *pop() override;
        packet::PicoPacket *pop() override;

        bool empty() override;
        void reset() override;

        std::string to_string() override;

        ~SortedBacklog() override;

    private:
        //std::deque<packet::FlowPacket *> _packets;
        std::deque<packet::PicoPacket *> _packets;
    };

} // collections

#endif //MULTIPATH_PROXY_BLOCKINGFLOWPACKETQUEUE_H
