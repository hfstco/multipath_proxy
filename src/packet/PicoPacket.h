//
// Created by Matthias Hofstätter on 24.08.23.
//

#ifndef MPP_PICOPACKET_H
#define MPP_PICOPACKET_H

#include <netinet/in.h>
#include <cstdlib>
#include <string>

namespace packet {

    inline size_t PicoPacketHeaderSize = sizeof(in_addr) + sizeof(in_addr) + sizeof(in_port_t) + sizeof(in_port_t) + sizeof(uint64_t) + sizeof(uint16_t);

    struct __attribute__((packed)) PicoPacket {
        struct in_addr source_ip;
        struct in_addr destination_ip;
        in_port_t source_port;
        in_port_t destination_port;
        uint64_t id;
        uint16_t size = 1024;
        uint8_t payload[1024]{};

        std::string to_string() {
            return "PicoPacket[id=" + std::to_string(id) + ", size=" + std::to_string(size) + "]"; // TODO source, destination
        }
    };

} // packet

#endif //MPP_PICOPACKET_H
