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

struct sockaddr_pair {
    sockaddr_pair(struct in_addr source_ip, in_port_t source_port, struct in_addr destination_ip, in_port_t destination_port) : source_ip_(source_ip),
                                                                                                                                source_port_(source_port),
                                                                                                                                destination_ip_(destination_port),
                                                                                                                                destination_port_(destination_port) {

    }

    bool operator==(const sockaddr_pair &other) const
    {
        return (source_ip_.s_addr == other.source_ip_.s_addr && source_port_ == other.source_port_ && destination_ip_.s_addr == other.destination_ip_.s_addr && destination_port_ == other.destination_port_);
        //return memcmp(this, &other, sizeof(SockAddr_In)); // TODO check
    }

    struct in_addr source_ip_;
    struct in_addr destination_ip_;
    in_port_t source_port_;
    in_port_t destination_port_;
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

template<>
struct std::hash<sockaddr_pair>
{
    std::size_t operator()(sockaddr_pair const& s) const noexcept
    {
        return s.source_ip_.s_addr ^ s.source_port_ ^ s.destination_ip_.s_addr ^ s.destination_port_; // TODO
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

//class FlowMap : private std::unordered_map<SockAddr_In_Pair, net::Flow *> { // TODO private unordered_map
class FlowMap : private std::unordered_map<sockaddr_pair, net::Flow *> {
    public:
        FlowMap();

        std::mutex &mutex();

        void addByteSize(uint64_t bytes) {
            _size += bytes;
        }

        void subByteSize(uint64_t bytes) {
            _size -= bytes;
        }

        uint64_t getByteSize() {
            return _size;
        }

        size_t Size();

        //void Insert(net::ipv4::SockAddr_In source, net::ipv4::SockAddr_In destination, net::Flow *flow);
        void Insert(in_addr source_ip, in_port_t source_port, in_addr destination_ip, in_port_t destination_port, net::Flow *flow);
        //net::Flow *Find(net::ipv4::SockAddr_In source, net::ipv4::SockAddr_In destination);
        net::Flow *Find(in_addr source_ip, in_port_t source_port, in_addr destination_ip, in_port_t destination_port);
        //void Erase(net::ipv4::SockAddr_In source, net::ipv4::SockAddr_In destination);
        void Erase(in_addr source_ip, in_port_t source_port, in_addr destination_ip, in_port_t destination_port);

        std::string ToString();

        virtual ~FlowMap();

    private:
        std::mutex _mutex;
        std::atomic<uint64_t> _size;
};

} // collections

#endif //MULTIPATH_PROXY_FLOWMAP_H
