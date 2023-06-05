//
// Created by Matthias Hofstätter on 13.04.23.
//

#ifndef MULTIPATH_PROXY_FLOWHEADER_H
#define MULTIPATH_PROXY_FLOWHEADER_H

#include <netinet/in.h>
#include <sstream>
#include "Header.h"

namespace net {
    namespace ipv4 {
        struct SockAddr_In;
    }
}

namespace packet {

        enum FLOW_CTRL : uint16_t {
            INIT = 0xFEED,
            REGULAR = 0xF00D,
            CLOSE = 0xDEAD
        };

        struct __attribute__((packed)) FlowHeader : public Header {
        public:
            FlowHeader() = delete;
            FlowHeader(net::ipv4::SockAddr_In source, net::ipv4::SockAddr_In destination, uint64_t id);

            FLOW_CTRL ctrl() const;
            void ctrl(FLOW_CTRL ctrl);

            struct in_addr sourceIp() const;
            void sourceIp(const in_addr sourceIp);

            struct in_addr destinationIp() const;
            void destinationIp(const in_addr destinationIp);

            in_port_t sourcePort() const;
            void sourcePort(in_port_t sourcePort);

            in_port_t destinationPort() const;
            void destinationPort(in_port_t destinationPort);

            uint64_t id() const;
            void id(uint64_t id);

            uint16_t size() const;
            void size(uint16_t size);

            /*uint64_t checksum() const;
            void checksum(uint64_t checksum);*/

            net::ipv4::SockAddr_In source() const;
            void source(net::ipv4::SockAddr_In sockAddrIn);

            net::ipv4::SockAddr_In destination() const;
            void destination(net::ipv4::SockAddr_In sockAddrIn);

            std::string ToString();

        protected:
            FlowHeader(FLOW_CTRL ctrl, net::ipv4::SockAddr_In source, net::ipv4::SockAddr_In destination, uint64_t id);

        private:
            // TYPE type;                       // 8
            FLOW_CTRL ctrl_;                    // 10
            struct in_addr sourceIP_;           // 14
            struct in_addr destinationIP_;      // 18
            in_port_t sourcePort_;              // 20
            in_port_t destinationPort_;         // 22
            uint64_t id_;                       // 24
            uint16_t size_;                     // 26
            //uint64_t checksum_;
        };

} // packet

#endif //MULTIPATH_PROXY_FLOWHEADER_H
