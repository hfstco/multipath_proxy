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
        Proxy(net::ipv4::SockAddr_In sock_addr_in);

        void accept();

        std::string to_string();

        virtual ~Proxy();

    private:
        net::ipv4::TcpListener *_tcp_listener;

        worker::Looper _accept_looper;
    };

} // net

#endif //MULTIPATH_PROXY_PROXY_H
