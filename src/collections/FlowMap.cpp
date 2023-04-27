//
// Created by Matthias Hofstätter on 17.04.23.
//

#include "FlowMap.h"

#include "../exception/Exception.h"
#include "../net/Flow.h"
#include <glog/logging.h>

namespace collections {

    FlowMap::FlowMap() : std::unordered_map<std::string, net::Flow *>() {}

    int FlowMap::size() {
        return std::unordered_map<std::string, net::Flow *>::size();
    }

    bool FlowMap::contains(std::string key) {
        return std::unordered_map<std::string, net::Flow *>::contains(key);
    }

    void FlowMap::insert(std::string key, net::Flow *flow) {
        LOG(ERROR) << "insert(" << key << ")";

        std::unordered_map<std::string, net::Flow *>::insert(std::pair<std::string, net::Flow *>(key, flow));
    }

    net::Flow *FlowMap::find(std::string key) { // TODO width ::at(key)? -> throws exception (https://cplusplus.com/reference/map/map/at/)
        LOG(ERROR) << "find(" << key << ")";

        std::unordered_map<std::string, net::Flow *>::iterator flow = std::unordered_map<std::string, net::Flow *>::find(key); // std::unordered_map<std::string, std::string>::at(utils::connectionString(header));
        if (flow == std::unordered_map<std::string, net::Flow *>::end()) {
            throw NotFoundException("key=" + key + " not found in map.");
        } else {
            return flow->second;
        }
    }

} // collections