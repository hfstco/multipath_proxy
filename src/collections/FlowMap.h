//
// Created by Matthias Hofstätter on 17.04.23.
//

#ifndef MULTIPATH_PROXY_FLOWMAP_H
#define MULTIPATH_PROXY_FLOWMAP_H

#include <unordered_map>
#include <string>
#include <mutex>
#include <atomic>

namespace net {
    class Flow;

    namespace ipv4 {
        struct SockAddr_In;
    }
}

namespace collections {

    class FlowMap : private std::unordered_map<std::string, net::Flow *> { // TODO private unordered_map
    public:
        FlowMap();

        const uint64_t byteSize() const;

        int Size();

        bool Contains(net::ipv4::SockAddr_In source, net::ipv4::SockAddr_In destination);
        void Insert(net::ipv4::SockAddr_In source, net::ipv4::SockAddr_In destination, net::Flow *flow);
        net::Flow *Find(net::ipv4::SockAddr_In source, net::ipv4::SockAddr_In destination);
        void Erase(net::ipv4::SockAddr_In source, net::ipv4::SockAddr_In destination);

        std::string ToString();

    private:
        std::mutex mutex_;
    };

} // collections

#endif //MULTIPATH_PROXY_FLOWMAP_H
