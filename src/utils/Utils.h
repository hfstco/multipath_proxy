//
// Created by Matthias Hofstätter on 04.04.23.
//

#ifndef MULTIPATH_PROXY_UTILS_H
#define MULTIPATH_PROXY_UTILS_H

#include <string>

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

} // mpp::utils


#endif //MULTIPATH_PROXY_UTILS_H
