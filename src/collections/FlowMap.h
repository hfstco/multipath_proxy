//
// Created by Matthias Hofstätter on 17.04.23.
//

#ifndef MULTIPATH_PROXY_FLOWMAP_H
#define MULTIPATH_PROXY_FLOWMAP_H

#include <unordered_map>
#include <string>
#include <mutex>
#include <atomic>

#include "../net/SockAddr.h"

struct SockAddr_In_Pair {
    SockAddr_In_Pair(net::ipv4::SockAddr_In source, net::ipv4::SockAddr_In destination) : source_(source), destination_(destination) {}

    bool operator==(const SockAddr_In_Pair &other) const
    {
        return (source_ == other.source_ && destination_ == other.destination_);
        //return memcmp(this, &other, sizeof(SockAddr_In)); // TODO check
    }

    net::ipv4::SockAddr_In source_;
    net::ipv4::SockAddr_In destination_;
};

template<>
struct std::hash<SockAddr_In_Pair>
{
    std::size_t operator()(SockAddr_In_Pair const& s) const noexcept
    {
        return s.source_.sin_addr.s_addr ^ s.source_.sin_port ^ s.destination_.sin_addr.s_addr ^ s.destination_.sin_port; // TODO
        return 0;
    }
};

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

class FlowMap : private std::unordered_map<SockAddr_In_Pair, net::Flow *> { // TODO private unordered_map
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
