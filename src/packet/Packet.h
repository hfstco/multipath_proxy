//
// Created by Matthias Hofstätter on 13.04.23.
//

#ifndef MULTIPATH_PROXY_PACKET_H
#define MULTIPATH_PROXY_PACKET_H

#include "Buffer.h"

namespace packet {

    class Header;

    struct Packet : public Buffer {
    public:
        packet::Header *header();

        uint16_t size();

        std::string ToString();

    protected:
        Packet(uint16_t size);
    };

} // packet

#endif //MULTIPATH_PROXY_PACKET_H
