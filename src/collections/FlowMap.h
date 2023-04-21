//
// Created by Matthias Hofstätter on 17.04.23.
//

#ifndef MULTIPATH_PROXY_FLOWMAP_H
#define MULTIPATH_PROXY_FLOWMAP_H

#include <unordered_map>
#include "../net/Flow.h"

namespace collections {

    class FlowMap : private std::unordered_map<std::string, net::Flow *> { // TODO private unordered_map
    public:
        FlowMap() : std::unordered_map<std::string, net::Flow *>() {}

        bool contains(std::string key) {
            return std::unordered_map<std::string, net::Flow *>::contains(key);
        }

        void insert(std::string key, net::Flow *flow) {
            std::unordered_map<std::string, net::Flow *>::insert(std::pair<std::string, net::Flow *>(key, flow));
        }

        net::Flow *find(std::string key) { // TODO width ::at(key)? -> throws exception (https://cplusplus.com/reference/map/map/at/)
            std::unordered_map<std::string, net::Flow *>::iterator flow = std::unordered_map<std::string, net::Flow *>::find(key); // std::unordered_map<std::string, std::string>::at(utils::connectionString(header));
            if (flow == std::unordered_map<std::string, net::Flow *>::end()) {
                throw NotFoundException("key=" + key + " not found in map.");
            } else {
                return flow->second;
            }
        }

        //erase

    };

} // collections

#endif //MULTIPATH_PROXY_FLOWMAP_H
