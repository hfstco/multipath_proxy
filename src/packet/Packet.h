//
// Created by Matthias Hofstätter on 13.04.23.
//

#ifndef MULTIPATH_PROXY_PACKET_H
#define MULTIPATH_PROXY_PACKET_H

#include "Buffer.h"

namespace packet {

    class Header;

    struct Packet : public packet::Buffer {
    public:
        Packet() = delete;

        packet::Header *header();
        unsigned char *data();
        uint16_t size();

        void Resize(uint16_t size);

        std::string ToString();

        ~Packet() override;

    protected:
        Packet(uint16_t size);
    };

} // packet

#endif //MULTIPATH_PROXY_PACKET_H
