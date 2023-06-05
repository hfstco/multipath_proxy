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

namespace packet {
    struct FlowPacket;
}

namespace collections {

    class FlowMap : private std::unordered_map<std::string, net::Flow *> { // TODO private unordered_map
    public:
        FlowMap();

        std::mutex &mutex();

        void addByteSize(uint64_t bytes) {
            byteSize_.fetch_add(bytes,std::memory_order_relaxed);
        }

        void subByteSize(uint64_t bytes) {
            byteSize_.fetch_sub(bytes,std::memory_order_relaxed);
        }

        uint64_t getByteSize() {
            return byteSize_.load(std::memory_order_relaxed);
        }

        int Size();

        void Insert(net::ipv4::SockAddr_In source, net::ipv4::SockAddr_In destination, net::Flow *flow);
        net::Flow *Find(net::ipv4::SockAddr_In source, net::ipv4::SockAddr_In destination);
        void Erase(net::ipv4::SockAddr_In source, net::ipv4::SockAddr_In destination);

        std::string ToString();

        virtual ~FlowMap();

    private:
        std::mutex mutex_;
        std::atomic<uint64_t> byteSize_;
    };

} // collections

#endif //MULTIPATH_PROXY_FLOWMAP_H
