//
// Created by Matthias Hofstätter on 04.04.23.
//

#include "Utils.h"

namespace utils {

    IpAndPort splitIPandPort(std::string ipandport) {
        int end = ipandport.find(':');
        return {ipandport.substr(0, end), std::stoi(ipandport.substr(end + 1, ipandport.length() - end))};
    }

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

    std::string ConnectionString(std::string sourceIp, unsigned short sourcePort, std::string destinationIp, unsigned short destinationPort) {
        return sourceIp + ":" + std::to_string(sourcePort) + "|" + destinationIp + ":" + std::to_string(destinationPort);
    }

    std::string ConnectionString(in_addr sourceIp, in_port_t sourcePort, in_addr destinationIp, in_port_t destinationPort) {
        return ConnectionString(InAddrToString(sourceIp), ntohs(sourcePort),
                                InAddrToString(destinationIp),ntohs(destinationPort));
    }

} // mpp::utils