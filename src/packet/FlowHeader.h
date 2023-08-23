//
// Created by Matthias Hofstätter on 13.04.23.
//

#ifndef MULTIPATH_PROXY_FLOWHEADER_H
#define MULTIPATH_PROXY_FLOWHEADER_H

#include <netinet/in.h>
#include <string>


namespace packet {

        struct __attribute__((packed)) FlowHeader {
            struct in_addr source_ip;           // 4
            struct in_addr destination_ip;      // 8
            in_port_t source_port;              // 10
            in_port_t destination_port;         // 12

            std::string to_string();
        };

} // packet

#endif //MULTIPATH_PROXY_FLOWHEADER_H
