//
// Created by Matthias Hofstätter on 13.04.23.
//

#ifndef MULTIPATH_PROXY_PACKET_H
#define MULTIPATH_PROXY_PACKET_H

#include "base/Buffer.h"
#include "header/Header.h"

namespace packet {

    struct Packet : public packet::Buffer {
    public:
        packet::header::Header *header() {
            return reinterpret_cast<packet::header::Header *>(packet::Buffer::data());
        };

        void resize(size_t size) {
            assert(size >= sizeof(header::Header)); // TODO exception
            packet::Buffer::resize(size);
        }

    protected:
        Packet(size_t size) : packet::Buffer(size) {
            assert(size >= sizeof(header::Header));
        }
    };

} // packet

#endif //MULTIPATH_PROXY_PACKET_H
