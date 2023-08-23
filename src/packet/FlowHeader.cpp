//
// Created by Matthias Hofstätter on 25.04.23.
//

#include "FlowHeader.h"

#include "../net/SockAddr.h"

namespace packet {

    std::string FlowHeader::to_string() {
        return "FlowHeader[source=" + net::ipv4::SockAddr_In(source_ip, source_port).to_string() + ", destination=" +
               net::ipv4::SockAddr_In(destination_ip, destination_port).to_string() + "]";
    }

} // packet
