//
// Created by Matthias Hofstätter on 17.04.23.
//

#ifndef MULTIPATH_PROXY_FLOWMAP_H
#define MULTIPATH_PROXY_FLOWMAP_H

#include <unordered_map>
#include <string>

namespace net {
    class Flow;
}

namespace collections {

    class FlowMap : public std::unordered_map<std::string, net::Flow *> { // TODO private unordered_map
    public:
        FlowMap();

        int size();

        bool contains(std::string key);

        void insert(std::string key, net::Flow *flow);

        net::Flow *find(std::string key);

        //erase
    };

} // collections

#endif //MULTIPATH_PROXY_FLOWMAP_H
