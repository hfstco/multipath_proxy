//
// Created by Matthias Hofstätter on 17.04.23.
//

#include <glog/logging.h>

#include "FlowMap.h"

#include "../exception/Exception.h"
#include "../net/Flow.h"
#include "../net/base/SockAddr.h"

namespace collections {

    FlowMap::FlowMap() : std::unordered_map<std::string, net::Flow *>() {}

    int FlowMap::Size() {
        return std::unordered_map<std::string, net::Flow *>::size();
    }

    bool FlowMap::Contains(net::ipv4::SockAddr_In source, net::ipv4::SockAddr_In destination) {
        return std::unordered_map<std::string, net::Flow *>::contains(source.ToString() + "|" + destination.ToString());
    }

    void FlowMap::Insert(net::ipv4::SockAddr_In source, net::ipv4::SockAddr_In destination, net::Flow *flow) {
        LOG(ERROR) << "insert(" << source.ToString() + "|" + destination.ToString() << ")";

        std::unordered_map<std::string, net::Flow *>::insert(std::pair<std::string, net::Flow *>(source.ToString() + "|" + destination.ToString(), flow));
    }



    net::Flow *FlowMap::Find(net::ipv4::SockAddr_In source, net::ipv4::SockAddr_In destination) { // TODO width ::at(key)? -> throws exception (https://cplusplus.com/reference/map/map/at/)
        //LOG(ERROR) << "find(" << source.ToString() + "|" + destination.ToString() << ")";

        std::unordered_map<std::string, net::Flow *>::iterator flow = std::unordered_map<std::string, net::Flow *>::find(source.ToString() + "|" + destination.ToString()); // std::unordered_map<std::string, std::string>::at(utils::connectionString(header));
        if (flow == std::unordered_map<std::string, net::Flow *>::end()) {
            throw NotFoundException("key=" + source.ToString() + "|" + destination.ToString() + " not found in map.");
        } else {
            return flow->second;
        }
    }

    std::string FlowMap::ToString() {
        return "FlowMap[Size=" + std::to_string(size()) + "]";
    }

    void FlowMap::Erase(net::ipv4::SockAddr_In source, net::ipv4::SockAddr_In destination) {
        std::unordered_map<std::string, net::Flow *>::iterator flow = std::unordered_map<std::string, net::Flow *>::find(source.ToString() + "|" + destination.ToString());
        if (flow == std::unordered_map<std::string, net::Flow *>::end()) {
            throw NotFoundException("key=" + source.ToString() + "|" + destination.ToString() + " not found in map.");
        } else {
            std::unordered_map<std::string, net::Flow *>::erase(flow);
        }
    }

    const uint64_t FlowMap::byteSize() const { // TODO without loop
        uint64_t byteSize = 0;

        for(auto &[key, flow] : (*this)) {
            byteSize += flow->byteSize();
        }

        return byteSize;
    }

} // collections