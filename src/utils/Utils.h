//
// Created by Matthias Hofstätter on 04.04.23.
//

#ifndef MPP_UTILS_UTILS_H
#define MPP_UTILS_UTILS_H

#include <string>
#include <arpa/inet.h>

namespace net {
    namespace ipv4 {
        struct SockAddr_In;
    }
}

namespace utils {

    std::string InAddrToString(in_addr &inAddr);

    std::string In6AddrToString(in6_addr &in6Addr);

    in_addr StringToInAddr(std::string ip);

    in6_addr StringToIn6Addr(std::string ip);

    std::string ConnectionString(net::ipv4::SockAddr_In source, net::ipv4::SockAddr_In destination);

} // mpp::utils


#endif //MPP_UTILS_UTILS_H
