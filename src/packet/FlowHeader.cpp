//
// Created by Matthias Hofstätter on 25.04.23.
//

#include "FlowHeader.h"

#include "../net/SockAddr.h"

namespace packet {

    std::string FlowHeader::ToString() {
        return "FlowHeader[source=" + net::ipv4::SockAddr_In(sourceIP, sourcePort).ToString() + ", destination=" + net::ipv4::SockAddr_In(destinationIP, destinationPort).ToString() + "]";
    }

} // packet
