//
// Created by Matthias Hofstätter on 17.04.23.
//

#ifndef MULTIPATH_PROXY_PROXY_H
#define MULTIPATH_PROXY_PROXY_H

#include "../worker/Looper.h"

namespace collections {
    class FlowMap;
}

namespace task {
    class ThreadPool;
}

namespace context {
    class Context;
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
        Proxy(net::ipv4::SockAddr_In sockAddrIn, net::Bond *bond, context::Context *context);

        void Accept();

        std::string ToString();

        virtual ~Proxy();

    private:
        net::ipv4::TcpListener *listener_;

        net::Bond *bond_;
        context::Context *context_;

        worker::Looper acceptLooper_;
    };

} // net

#endif //MULTIPATH_PROXY_PROXY_H
