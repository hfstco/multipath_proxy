//
// Created by Matthias Hofstätter on 13.04.23.
//

#ifndef MULTIPATH_PROXY_FLOWHEADER_H
#define MULTIPATH_PROXY_FLOWHEADER_H

#include <netinet/in.h>
#include "Header.h"
#include "../../net/base/SockAddr.h"

namespace packet {
    namespace header {

        enum FLOW_CTRL : uint16_t {
            INIT = 0xFEED,
            REGULAR = 0xF00D,
            CLOSE = 0xDEAD
        };

        struct __attribute__((packed)) FlowHeader : public Header {
        public:
            FlowHeader() = delete;
            FlowHeader(net::ipv4::SockAddr_In source, net::ipv4::SockAddr_In destination, uint16_t id) : FlowHeader(FLOW_CTRL::REGULAR,
                                                                                                       source, destination, id) {}

            FLOW_CTRL ctrl() const { // TODO setter access
                return ctrl_;
            }

            void ctrl(FLOW_CTRL ctrl) {
                ctrl_ = ctrl;
            }

            struct in_addr sourceIp() const {
                return sourceIP_;
            }

            void sourceIp(const in_addr sourceIp) {
                sourceIP_ = sourceIp;
            }

            struct in_addr destinationIp() const {
                return destinationIP_;
            }

            void destinationIp(const in_addr destinationIp) {
                destinationIP_ = destinationIp;
            }

            in_port_t sourcePort() const {
                return sourcePort_;
            }

            void sourcePort(in_port_t sourcePort) {
                sourcePort_ = sourcePort;
            }

            in_port_t destinationPort() const {
                return destinationPort_;
            }

            void destinationPort(in_port_t destinationPort) {
                destinationPort_ = destinationPort;
            }

            uint16_t id() const {
                return id_;
            }

            void id(uint16_t id) {
                id_ = id;
            }

            uint16_t size() const {
                return size_;
            }

            void size(uint16_t size) {
                size_ = size;
            }

        protected:
            FlowHeader(FLOW_CTRL ctrl, net::ipv4::SockAddr_In source, net::ipv4::SockAddr_In destination, uint16_t id) : Header(TYPE::FLOW), ctrl_(ctrl), sourceIP_(source.sin_addr),
                                                                                                            destinationIP_(destination.sin_addr), sourcePort_(source.sin_port),
                                                                                                            destinationPort_(destination.sin_port), id_(id), size_(0) {}

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

        struct FlowCloseHeader : FlowHeader {
            FlowCloseHeader(net::ipv4::SockAddr_In source, net::ipv4::SockAddr_In destination, uint16_t id) : FlowHeader(FLOW_CTRL::CLOSE,
                                                                                                            source, destination, id) {}
        };

        struct FlowInitHeader : FlowHeader {
            FlowInitHeader(net::ipv4::SockAddr_In source, net::ipv4::SockAddr_In destination) : FlowHeader(FLOW_CTRL::INIT,
                                                                                                           source, destination, 0) {}
        };

    } // header
} // packet

#endif //MULTIPATH_PROXY_FLOWHEADER_H
