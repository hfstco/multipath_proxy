//
// Created by Matthias Hofstätter on 02.05.23.
//

#ifndef MULTIPATH_PROXY_ENDPOINT_H
#define MULTIPATH_PROXY_ENDPOINT_H


#include <cstdint>
#include <string>
#include <sstream>

class Endpoint {
public:
    Endpoint(std::string endpoint) {
        int colon = endpoint.find(':');
        ip_ = endpoint.substr(0, colon);
        port_ = std::stoi(endpoint.substr(colon + 1, endpoint.length() - colon));
    }

    Endpoint(std::string ip, uint16_t port) : ip_(ip), port_(port) {}

    const std::string &ip() const {
        return ip_;
    }

    void ip(const std::string &ip) {
        ip_ = ip;
    }

    uint16_t port() const {
        return port_;
    }

    void port(uint16_t port) {
        port_ = port;
    }

    std::string ToString() {
        std::stringstream stringStream;
        stringStream << ip_ << ":" << std::to_string(port_);
        return stringStream.str();
    }

private:
    std::string ip_;
    uint16_t port_;
};


#endif //MULTIPATH_PROXY_ENDPOINT_H
