//
// Created by Matthias Hofstätter on 17.04.23.
//

#ifndef MULTIPATH_PROXY_PROXY_H
#define MULTIPATH_PROXY_PROXY_H

#include <atomic>
#include <thread>

namespace collections {
    class FlowMap;
}

namespace net {

    namespace ipv4 {
        class SockAddr_In;
        class TcpListener;
    }

    class Bond;

    class Proxy {
    public:
        Proxy() = delete;

        static Proxy *make(net::ipv4::SockAddr_In sockAddrIn, collections::FlowMap *pFlows, net::Bond *pBond);

        virtual ~Proxy();

    private:
        net::ipv4::TcpListener *listener_;
        collections::FlowMap *flows_;
        net::Bond *bond_;

        std::atomic_bool stop_; // TODO implement Handler
        std::thread *acceptHandler_; // TODO implement Handler

        Proxy(net::ipv4::SockAddr_In sockAddrIn, collections::FlowMap *flows, net::Bond *pBond);

        void AcceptFromConnection();
    };

} // net

#endif //MULTIPATH_PROXY_PROXY_H
