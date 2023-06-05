//
// Created by Matthias Hofstätter on 14.04.23.
//

#ifndef MULTIPATH_PROXY_FLOWPACKET_H
#define MULTIPATH_PROXY_FLOWPACKET_H

#include "Packet.h"

namespace packet {

    struct FlowHeader;

    struct FlowPacket : public packet::Packet {
    public:
        FlowPacket() = delete;

        static FlowPacket *make(FlowHeader header, uint16_t size);
        static FlowPacket *make(FlowHeader header);

        FlowHeader *header();
        unsigned char *data();
        uint16_t size();

        void Resize(uint16_t size);

        std::string ToString();

        //static uint64_t CalcChecksum(FlowPacket *flowPacket);

        ~FlowPacket() override;

    protected:
        FlowPacket(FlowHeader flowHeader, uint16_t size);
        FlowPacket(FlowHeader flowHeader);
    }; // packet::FlowPacket

} // packet

#endif //MULTIPATH_PROXY_FLOWPACKET_H
