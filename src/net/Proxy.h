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

    class QuicConnection;

    class Proxy {
    public:
        Proxy() = delete;
        Proxy(net::ipv4::SockAddr_In sockAddrIn);

        void Accept();

        std::string ToString();

        virtual ~Proxy();

    private:
        net::ipv4::TcpListener *_tcpListener;
        net::QuicConnection *_TER;
        net::QuicConnection *_SAT;

        worker::Looper _acceptLooper;
    };

} // net

#endif //MULTIPATH_PROXY_PROXY_H
