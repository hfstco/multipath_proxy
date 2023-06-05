//
// Created by Matthias Hofstätter on 17.04.23.
//

#include <glog/logging.h>

#include "FlowMap.h"

#include "../exception/Exception.h"
#include "../net/Flow.h"
#include "../net/base/SockAddr.h"
#include "../packet/FlowPacket.h"
#include "../packet/FlowHeader.h"

namespace collections {

    FlowMap::FlowMap() : std::unordered_map<std::string, net::Flow *>() {}

    int FlowMap::Size() {
        return std::unordered_map<std::string, net::Flow *>::size();
    }

    void FlowMap::Insert(net::ipv4::SockAddr_In source, net::ipv4::SockAddr_In destination, net::Flow *flow) {
        LOG(ERROR) << ToString() <<  ".Insert(" << source.ToString() + ", " + destination.ToString() << ")";

        std::unordered_map<std::string, net::Flow *>::insert(std::pair<std::string, net::Flow *>(source.ToString() + "|" + destination.ToString(), flow));
    }

    net::Flow *FlowMap::Find(net::ipv4::SockAddr_In source, net::ipv4::SockAddr_In destination) { // TODO width ::at(key)? -> return nullptr (https://cplusplus.com/reference/map/map/at/)
        std::unordered_map<std::string, net::Flow *>::iterator flow = std::unordered_map<std::string, net::Flow *>::find(source.ToString() + "|" + destination.ToString()); // std::unordered_map<std::string, std::string>::at(utils::connectionString(header));
        if (flow == std::unordered_map<std::string, net::Flow *>::end()) {
            return nullptr;
        } else {
            return flow->second;
        }
    }

    std::string FlowMap::ToString() {
        return "FlowMap[Size=" + std::to_string(size()) + "]"; // TODO print flows
    }

    void FlowMap::Erase(net::ipv4::SockAddr_In source, net::ipv4::SockAddr_In destination) {
        std::unordered_map<std::string, net::Flow *>::iterator flow = std::unordered_map<std::string, net::Flow *>::find(source.ToString() + "|" + destination.ToString());
        if (flow == std::unordered_map<std::string, net::Flow *>::end()) {
            NotFoundException e = NotFoundException("key=" + source.ToString() + "|" + destination.ToString() + " not found in map.");
            LOG(INFO) << ToString() << ".Erase(" << source.ToString() << "|" << destination.ToString() << ") ~> " << e.ToString();

            throw e;
        } else {
            LOG(INFO) << ToString() << ".Erase(" << flow->second->ToString() << ")";

            std::unordered_map<std::string, net::Flow *>::erase(flow);
        }
    }

    FlowMap::~FlowMap() {
        DLOG(INFO) << ToString() << ".~FlowMap()";
    }

    std::mutex &FlowMap::mutex() {
        return mutex_;
    }

} // collections