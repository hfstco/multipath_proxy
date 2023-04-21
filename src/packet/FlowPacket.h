//
// Created by Matthias Hofstätter on 14.04.23.
//

#ifndef MULTIPATH_PROXY_FLOWPACKET_H
#define MULTIPATH_PROXY_FLOWPACKET_H

#include "Packet.h"
#include "header/FlowHeader.h"

namespace packet {

    struct FlowPacket : public Packet {
    public:
        static FlowPacket *make(packet::header::FlowHeader header, uint16_t size) {
            return new FlowPacket(header, size);
        }

        static FlowPacket *make(packet::header::FlowHeader header) {
            return new FlowPacket(header, 0);
        }

        static FlowPacket *make(packet::header::FlowHeader header, unsigned char *data, uint16_t size) {
            return new FlowPacket(header, data, size);
        }

        packet::header::FlowHeader *header() {
            return reinterpret_cast<packet::header::FlowHeader *>(Packet::header());
        }

        unsigned char *data() {
            return ((unsigned char *)Packet::header()) + sizeof(packet::header::FlowHeader);
        }

        void resize(size_t size) {
            Packet::resize(sizeof(packet::header::FlowHeader) + size);
            this->header()->size(size);
        }

    protected:
        FlowPacket(packet::header::FlowHeader header, uint16_t size) : Packet(sizeof(packet::header::FlowHeader) + size) {
            header.size(size);
            memcpy(this->header(), &header, sizeof(packet::header::FlowHeader));
        }

        FlowPacket(packet::header::FlowHeader header) : FlowPacket(header, 0) {}

        FlowPacket(packet::header::FlowHeader header, unsigned char *data, uint16_t size) : FlowPacket(header, size) {
            memcpy(this->data(), data, size);
        }
    }; // packet::FlowPacket

} // packet

#endif //MULTIPATH_PROXY_FLOWPACKET_H
