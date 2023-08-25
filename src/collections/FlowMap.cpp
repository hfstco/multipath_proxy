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

    //FlowMap::FlowMap() : std::unordered_map<SockAddr_In_Pair, net::Flow *>() {}
    FlowMap::FlowMap() : std::unordered_map<sockaddr_pair, net::Flow *>() {}

    size_t FlowMap::Size() {
        //return std::unordered_map<SockAddr_In_Pair, net::Flow *>::size();
        return std::unordered_map<sockaddr_pair, net::Flow *>::size();
    }

    /*void FlowMap::Insert(net::ipv4::SockAddr_In source, net::ipv4::SockAddr_In destination, net::Flow *flow) {
        //LOG(ERROR) << ToString() <<  ".Insert(" << source.ToString() + ", " + destination.ToString() << ")";

        //std::unordered_map<SockAddr_In_Pair, net::Flow *>::insert(std::pair<SockAddr_In_Pair, net::Flow *>(SockAddr_In_Pair(source, destination), flow));
        std::unordered_map<sockaddr_pair, net::Flow *>::insert(std::pair<sockaddr_pair, net::Flow *>(sockaddr_pair(source.sin_addr, source.sin_port, destination.sin_addr, destination.sin_port), flow));
    }*/

    void FlowMap::Insert(in_addr source_ip, in_port_t source_port, in_addr destination_ip, in_port_t destination_port, net::Flow *flow) {
        //LOG(ERROR) << ToString() <<  ".Insert(" << source.ToString() + ", " + destination.ToString() << ")";

        //std::unordered_map<SockAddr_In_Pair, net::Flow *>::insert(std::pair<SockAddr_In_Pair, net::Flow *>(SockAddr_In_Pair(source, destination), flow));
        std::unordered_map<sockaddr_pair, net::Flow *>::insert(std::pair<sockaddr_pair, net::Flow *>(sockaddr_pair(source_ip, source_port, destination_ip, destination_port), flow));
    }

    /*net::Flow *FlowMap::Find(net::ipv4::SockAddr_In source, net::ipv4::SockAddr_In destination) { // TODO width ::at(key)? -> return nullptr (https://cplusplus.com/reference/map/map/at/)
        std::unordered_map<SockAddr_In_Pair, net::Flow *>::iterator flow = std::unordered_map<SockAddr_In_Pair, net::Flow *>::find(SockAddr_In_Pair(source, destination)); // std::unordered_map<std::string, std::string>::at(utils::connectionString(header));
        if (flow == std::unordered_map<SockAddr_In_Pair, net::Flow *>::end()) {
            return nullptr;
        } else {
            return flow->second;
        }
    }*/

    net::Flow *FlowMap::Find(in_addr source_ip, in_port_t source_port, in_addr destination_ip, in_port_t destination_port) { // TODO width ::at(key)? -> return nullptr (https://cplusplus.com/reference/map/map/at/)
        auto flow = std::unordered_map<sockaddr_pair, net::Flow *>::find(sockaddr_pair(source_ip, source_port, destination_ip, destination_port)); // std::unordered_map<std::string, std::string>::at(utils::connectionString(header));
        if (flow == std::unordered_map<sockaddr_pair, net::Flow *>::end()) {
            return nullptr;
        } else {
            return flow->second;
        }
    }

    std::string FlowMap::ToString() {
        return "FlowMap[Size=" + std::to_string(size()) + "]"; // TODO print flows
    }

    /*void FlowMap::Erase(net::ipv4::SockAddr_In source, net::ipv4::SockAddr_In destination) {
        std::unordered_map<SockAddr_In_Pair, net::Flow *>::iterator flow = std::unordered_map<SockAddr_In_Pair, net::Flow *>::find(SockAddr_In_Pair(source, destination));
        if (flow == std::unordered_map<SockAddr_In_Pair, net::Flow *>::end()) {
            NotFoundException e = NotFoundException("key=" + source.to_string() + "|" + destination.to_string() + " not found in map.");
            //LOG(INFO) << ToString() << ".Erase(" << source.ToString() << "|" << destination.ToString() << ") ~> " << e.ToString();

            throw e;
        } else {
            //LOG(INFO) << ToString() << ".Erase(" << flow->second->ToString() << ")";

            std::unordered_map<SockAddr_In_Pair, net::Flow *>::erase(flow);
        }
    }*/

    void FlowMap::Erase(in_addr source_ip, in_port_t source_port, in_addr destination_ip, in_port_t destination_port) {
        auto flow = std::unordered_map<sockaddr_pair, net::Flow *>::find(sockaddr_pair(source_ip, source_port, destination_ip, destination_port));
        if (flow == std::unordered_map<sockaddr_pair, net::Flow *>::end()) {
            NotFoundException e = NotFoundException("");
            //LOG(INFO) << ToString() << ".Erase(" << source.ToString() << "|" << destination.ToString() << ") ~> " << e.ToString();

            throw e;
        } else {
            //LOG(INFO) << ToString() << ".Erase(" << flow->second->ToString() << ")";

            std::unordered_map<sockaddr_pair, net::Flow *>::erase(flow);
        }
    }

    FlowMap::~FlowMap() {
        //DLOG(INFO) << ToString() << ".~FlowMap()";
    }

    std::mutex &FlowMap::mutex() {
        return _mutex;
    }

} // collections