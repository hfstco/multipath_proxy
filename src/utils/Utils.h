//
// Created by Matthias Hofstätter on 04.04.23.
//

#ifndef MULTIPATH_PROXY_UTILS_H
#define MULTIPATH_PROXY_UTILS_H

#include <string>
#include "../net/base/TcpConnection.h"
#include "../packet/header/FlowHeader.h"

namespace utils {

    struct IpAndPort {
        std::string ip;
        int port;
    };

    static IpAndPort splitIPandPort(std::string ipandport);
    IpAndPort splitIPandPort(std::string ipandport) {
        int end = ipandport.find(':');
        return {ipandport.substr(0, end), std::stoi(ipandport.substr(end + 1, ipandport.length() - end))};
    }

    static std::string connectionString(std::string sourceIp, unsigned short sourcePort, std::string destinationIp, unsigned short destinationPort) {
        return sourceIp + ":" + std::to_string(sourcePort) + "|" + destinationIp + ":" + std::to_string(destinationPort);
    }

    static std::string connectionString(packet::header::FlowHeader &flowHeader) {
        in_addr sip = flowHeader.sourceIp();
        in_addr dip = flowHeader.destinationIp();
        char sourceIp[INET_ADDRSTRLEN], destinationIp[INET_ADDRSTRLEN];

        inet_ntop(AF_INET, (const void *)&sip, sourceIp, INET_ADDRSTRLEN);
        inet_ntop(AF_INET, (const void *)&dip, destinationIp, INET_ADDRSTRLEN);
        return connectionString(sourceIp, htons(flowHeader.sourcePort()), destinationIp, htons(flowHeader.destinationPort()));
    }

    static std::string connectionString(net::ipv4::SockAddr_In sourceSockAddr_In, net::ipv4::SockAddr_In destinationSockAddr_In) {
        return connectionString(sourceSockAddr_In.ip(), sourceSockAddr_In.port(), destinationSockAddr_In.ip(), destinationSockAddr_In.port());
    }

} // mpp::utils


#endif //MULTIPATH_PROXY_UTILS_H
