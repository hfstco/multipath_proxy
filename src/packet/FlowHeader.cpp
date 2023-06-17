//
// Created by Matthias Hofstätter on 25.04.23.
//

#include "FlowHeader.h"

#include "../net/SockAddr.h"

namespace packet {
    FlowHeader::FlowHeader(FLOW_CTRL ctrl, net::ipv4::SockAddr_In source, net::ipv4::SockAddr_In destination, uint64_t id) : Header(TYPE::FLOW), ctrl_(ctrl), sourceIP_(source.sin_addr),
    destinationIP_(destination.sin_addr), sourcePort_(source.sin_port),
    destinationPort_(destination.sin_port), id_(id), size_(0)/*, checksum_(0)*/ {}

    FlowHeader::FlowHeader(net::ipv4::SockAddr_In source, net::ipv4::SockAddr_In destination, uint64_t id) : FlowHeader(FLOW_CTRL::REGULAR,
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

    uint64_t FlowHeader::id() const {
        return id_;
    }

    void FlowHeader::id(uint64_t id) {
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
        stringStream << "FlowHeader[ctrl=";
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
        stringStream << ", source=" << source().ToString() << ", destination=" << destination().ToString() << ", id=" << id() << ", Size=" << size() << "]";

        return stringStream.str();
    }

    net::ipv4::SockAddr_In FlowHeader::source() const {
        return net::ipv4::SockAddr_In(sourceIP_, sourcePort_);
    }

    void FlowHeader::source(net::ipv4::SockAddr_In sockAddrIn) {
        sourceIP_ = sockAddrIn.sin_addr;
        sourcePort_ = sockAddrIn.sin_port;
    }

    net::ipv4::SockAddr_In FlowHeader::destination() const {
        return net::ipv4::SockAddr_In(destinationIP_, destinationPort_);
    }

    void FlowHeader::destination(net::ipv4::SockAddr_In sockAddrIn) {
        destinationIP_ = sockAddrIn.sin_addr;
        destinationPort_ = sockAddrIn.sin_port;
    }

    /*uint64_t FlowHeader::checksum() const {
        return checksum_;
    }

    void FlowHeader::checksum(uint64_t checksum) {
        checksum_ = checksum;
    };*/

} // packet
