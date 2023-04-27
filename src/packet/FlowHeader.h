//
// Created by Matthias Hofstätter on 13.04.23.
//

#ifndef MULTIPATH_PROXY_FLOWHEADER_H
#define MULTIPATH_PROXY_FLOWHEADER_H

#include <netinet/in.h>
#include <sstream>
#include "Header.h"
#include "../net/base/SockAddr.h"

namespace packet {

        enum FLOW_CTRL : uint16_t {
            INIT = 0xFEED,
            REGULAR = 0xF00D,
            CLOSE = 0xDEAD
        };

        struct __attribute__((packed)) FlowHeader : public Header {
        public:
            FlowHeader() = delete;
            FlowHeader(net::ipv4::SockAddr_In source, net::ipv4::SockAddr_In destination, uint16_t id);

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

            uint16_t id() const;
            void id(uint16_t id);

            uint16_t size() const;
            void size(uint16_t size);

            std::string ToString();

        protected:
            FlowHeader(FLOW_CTRL ctrl, net::ipv4::SockAddr_In source, net::ipv4::SockAddr_In destination, uint16_t id);

        private:
            // uint32_t magic = MAGIC;          // 4
            // TYPE type;                       // 8
            FLOW_CTRL ctrl_;                    // 10
            struct in_addr sourceIP_;           // 14
            struct in_addr destinationIP_;      // 18
            in_port_t sourcePort_;              // 20
            in_port_t destinationPort_;         // 22
            uint16_t id_;                       // 24
            uint16_t size_;                     // 26
        };

        struct FlowCloseHeader : public FlowHeader {
            FlowCloseHeader(net::ipv4::SockAddr_In source, net::ipv4::SockAddr_In destination, uint16_t id) : FlowHeader(FLOW_CTRL::CLOSE,
                                                                                                            source, destination, id) {}
        };

        struct FlowInitHeader : public FlowHeader {
            FlowInitHeader(net::ipv4::SockAddr_In source, net::ipv4::SockAddr_In destination) : FlowHeader(FLOW_CTRL::INIT,
                                                                                                           source, destination, 0) {}
        };

} // packet

#endif //MULTIPATH_PROXY_FLOWHEADER_H
