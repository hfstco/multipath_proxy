//
// Created by Matthias Hofstätter on 17.04.23.
//

#ifndef MULTIPATH_PROXY_PROXY_H
#define MULTIPATH_PROXY_PROXY_H

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

        static Proxy *make(net::ipv4::SockAddr_In sockAddrIn);

        void Accept(collections::FlowMap *flows, net::Bond *bond);

        virtual ~Proxy();

    protected:
        Proxy(net::ipv4::SockAddr_In sockAddrIn);

    private:
        net::ipv4::TcpListener *listener_;
    };

} // net

#endif //MULTIPATH_PROXY_PROXY_H
