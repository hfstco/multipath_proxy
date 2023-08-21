//
// Created by Matthias Hofstätter on 13.04.23.
//

#ifndef MULTIPATH_PROXY_FLOWHEADER_H
#define MULTIPATH_PROXY_FLOWHEADER_H

#include <netinet/in.h>
#include <string>


namespace packet {

        struct __attribute__((packed)) FlowHeader {
            struct in_addr sourceIP;           // 4
            struct in_addr destinationIP;      // 8
            in_port_t sourcePort;              // 10
            in_port_t destinationPort;         // 12

            std::string ToString();
        };

} // packet

#endif //MULTIPATH_PROXY_FLOWHEADER_H
