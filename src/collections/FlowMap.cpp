//
// Created by Matthias Hofstätter on 17.04.23.
//

#include <glog/logging.h>

#include "FlowMap.h"

#include "../exception/Exception.h"
#include "../net/Flow.h"
#include "../net/SockAddr.h"
#include "../packet/FlowPacket.h"
#include "../packet/FlowHeader.h"

namespace collections {

    FlowMap::FlowMap() : std::unordered_map<SockAddr_In_Pair, net::Flow *>() {}

    int FlowMap::Size() {
        return std::unordered_map<SockAddr_In_Pair, net::Flow *>::size();
    }

    // insert new flow to map
    void FlowMap::Insert(net::ipv4::SockAddr_In source, net::ipv4::SockAddr_In destination, net::Flow *flow) {
        std::unordered_map<SockAddr_In_Pair, net::Flow *>::insert(std::pair<SockAddr_In_Pair, net::Flow *>(SockAddr_In_Pair(source, destination), flow));
    }

    // find flow in map
    net::Flow *FlowMap::Find(net::ipv4::SockAddr_In source, net::ipv4::SockAddr_In destination) { // TODO width ::at(key)? -> return nullptr (https://cplusplus.com/reference/map/map/at/)
        std::unordered_map<SockAddr_In_Pair, net::Flow *>::iterator flow = std::unordered_map<SockAddr_In_Pair, net::Flow *>::find(SockAddr_In_Pair(source, destination)); // std::unordered_map<std::string, std::string>::at(utils::connectionString(header));
        if (flow == std::unordered_map<SockAddr_In_Pair, net::Flow *>::end()) {
            // return nullptr if flow doesn't exist
            return nullptr;
        } else {
            return flow->second;
        }
    }

    std::string FlowMap::ToString() {
        return "FlowMap[Size=" + std::to_string(size()) + "]"; // TODO print flows
    }

    // erase flow from map
    void FlowMap::Erase(net::ipv4::SockAddr_In source, net::ipv4::SockAddr_In destination) {
        std::unordered_map<SockAddr_In_Pair, net::Flow *>::iterator flow = std::unordered_map<SockAddr_In_Pair, net::Flow *>::find(SockAddr_In_Pair(source, destination));
        if (flow == std::unordered_map<SockAddr_In_Pair, net::Flow *>::end()) { // if flow doesn't exist throw exception
            NotFoundException e = NotFoundException("key=" + source.ToString() + "|" + destination.ToString() + " not found in map.");

            throw e;
        } else {
            std::unordered_map<SockAddr_In_Pair, net::Flow *>::erase(flow);
        }
    }

    FlowMap::~FlowMap() {
        //DLOG(INFO) << ToString() << ".~FlowMap()";
    }

    std::mutex &FlowMap::mutex() {
        return mutex_;
    }

} // collections