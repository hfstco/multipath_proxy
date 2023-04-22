//
// Created by Matthias Hofstätter on 04.04.23.
//

#ifndef MPP_UTILS_UTILS_H
#define MPP_UTILS_UTILS_H

#include <string>
#include <arpa/inet.h>
#include "../exception/Exception.h"

namespace utils {

    struct IpAndPort {
        std::string ip;
        int port;
    };

    IpAndPort splitIPandPort(std::string ipandport);

    std::string InAddrToString(in_addr &inAddr);

    std::string In6AddrToString(in6_addr &in6Addr);

    in_addr StringToInAddr(std::string ip);

    in6_addr StringToIn6Addr(std::string ip);

    std::string ConnectionString(std::string sourceIp, unsigned short sourcePort, std::string destinationIp, unsigned short destinationPort);

    std::string ConnectionString(in_addr sourceIp, in_port_t sourcePort, in_addr destinationIp, in_port_t destinationPort);

} // mpp::utils


#endif //MPP_UTILS_UTILS_H
