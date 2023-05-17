//
// Created by Matthias Hofstätter on 04.04.23.
//

#include <cstring>

#include "Utils.h"

#include "../exception/Exception.h"
#include "../net/base/SockAddr.h"

namespace utils {

    std::string InAddrToString(in_addr &inAddr) {
        int result;
        char ip[INET_ADDRSTRLEN];

        if (!inet_ntop(AF_INET, &(inAddr.s_addr), ip, INET_ADDRSTRLEN)) {
            throw UtilsException("inet_ntop() failed. errno=" + std::string(strerror(errno)));
        }

        return std::string(ip);
    }

    std::string In6AddrToString(in6_addr &in6Addr) {
        int result;
        char ip[INET6_ADDRSTRLEN];

        if (!inet_ntop(AF_INET6, &(in6Addr.s6_addr), ip, INET6_ADDRSTRLEN)) {
            throw UtilsException("inet_ntop() failed. errno=" + std::string(strerror(errno)));
        }

        return std::string(ip);
    }

    in_addr StringToInAddr(std::string ip) {
        int result;
        in_addr inAddr;

        if ((result = inet_pton(AF_INET, ip.c_str(), &(inAddr.s_addr))) <= 0) {
            if (result == 0) {
                throw UtilsException("not a valid ipv4 address.");
            }
            throw UtilsException("inet_pton() failed. errno=" + std::string(strerror(errno)));
        }

        return inAddr;
    }

    in6_addr StringToIn6Addr(std::string ip) {
        int result;
        in6_addr in6Addr;

        if ((result = inet_pton(AF_INET6, ip.c_str(), &(in6Addr.s6_addr))) <= 0) {
            if (result == 0) {
                throw UtilsException("not a valid ipv6 address.");
            }
            throw UtilsException("inet_pton() failed. errno=" + std::string(strerror(errno)));
        }

        return in6Addr;
    }

    std::string ConnectionString(net::ipv4::SockAddr_In source, net::ipv4::SockAddr_In destination) {
        return source.ip() + ":" + std::to_string(source.port()) + "|" + destination.ip() + ":" + std::to_string(destination.port());
    }

} // mpp::utils