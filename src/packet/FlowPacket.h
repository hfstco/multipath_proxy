//
// Created by Matthias Hofstätter on 14.04.23.
//

#ifndef MULTIPATH_PROXY_FLOWPACKET_H
#define MULTIPATH_PROXY_FLOWPACKET_H

#include "Packet.h"

namespace packet {

    class FlowHeader;

    struct FlowPacket : public Packet {
    public:
        static FlowPacket *make(packet::FlowHeader header, uint16_t size);
        static FlowPacket *make(packet::FlowHeader header);
        static FlowPacket *make(packet::FlowHeader header, unsigned char *data, uint16_t size);

        packet::FlowHeader *header();
        unsigned char *data();

        uint16_t size();
        void resize(size_t size);

        std::string ToString();

        ~FlowPacket() override;

    protected:
        FlowPacket(packet::FlowHeader header, uint16_t size);
        FlowPacket(packet::FlowHeader header);
        FlowPacket(packet::FlowHeader header, unsigned char *data, uint16_t size);
    }; // packet::FlowPacket

} // packet

#endif //MULTIPATH_PROXY_FLOWPACKET_H
