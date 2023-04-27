//
// Created by Matthias Hofstätter on 25.04.23.
//

#include "FlowHeader.h"

namespace packet {
    FlowHeader::FlowHeader(FLOW_CTRL ctrl, net::ipv4::SockAddr_In source, net::ipv4::SockAddr_In destination, uint16_t id) : Header(TYPE::FLOW), ctrl_(ctrl), sourceIP_(source.sin_addr),
    destinationIP_(destination.sin_addr), sourcePort_(source.sin_port),
    destinationPort_(destination.sin_port), id_(id), size_(0) {}

    FlowHeader::FlowHeader(net::ipv4::SockAddr_In source, net::ipv4::SockAddr_In destination, uint16_t id) : FlowHeader(FLOW_CTRL::REGULAR,
            source, destination, id) {}

    FLOW_CTRL FlowHeader::ctrl() const {
        return ctrl_;
    }

    void FlowHeader::ctrl(FLOW_CTRL ctrl) {
        ctrl_ = ctrl;
    }

    struct in_addr FlowHeader::sourceIp() const {
        return sourceIP_;
    }

    void FlowHeader::sourceIp(const in_addr sourceIp) {
        sourceIP_ = sourceIp;
    }

    struct in_addr FlowHeader::destinationIp() const {
        return destinationIP_;
    }

    void FlowHeader::destinationIp(const in_addr destinationIp) {
        destinationIP_ = destinationIp;
    }

    in_port_t FlowHeader::sourcePort() const {
        return sourcePort_;
    }

    void FlowHeader::sourcePort(in_port_t sourcePort) {
        sourcePort_ = sourcePort;
    }

    in_port_t FlowHeader::destinationPort() const {
        return destinationPort_;
    }

    void FlowHeader::destinationPort(in_port_t destinationPort) {
        destinationPort_ = destinationPort;
    }

    uint16_t FlowHeader::id() const {
        return id_;
    }

    void FlowHeader::id(uint16_t id) {
        id_ = id;
    }

    uint16_t FlowHeader::size() const {
        return size_;
    }

    void FlowHeader::size(uint16_t size) {
        size_ = size;
    }

    std::string FlowHeader::ToString() {
        std::stringstream stringStream;
        stringStream << "FlowPacket[ctrl=";
        switch (ctrl_) {
            case FLOW_CTRL::INIT:
                stringStream << "INIT";
                break;
            case FLOW_CTRL::REGULAR:
                stringStream << "REGULAR";
                break;
            case FLOW_CTRL::CLOSE:
                stringStream << "CLOSE";
                break;
        }
        stringStream << ", id=" << id() << ", size=" << size() << "]";
        return stringStream.str();
    };

} // packet
