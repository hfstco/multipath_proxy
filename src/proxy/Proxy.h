//
// Created by Matthias Hofstätter on 17.04.23.
//

#ifndef MULTIPATH_PROXY_PROXY_H
#define MULTIPATH_PROXY_PROXY_H

#include "../worker/Looper.h"
#include "../net/SockAddr.h"
#include "../quic/Context.h"

namespace net::ipv4 {
    class TcpListener;
}

namespace quic {
    class FlowContext;
}

namespace proxy {

    class Proxy {
    public:
        Proxy() = delete;
        Proxy(quic::FlowContext *context, net::ipv4::SockAddr_In sock_addr_in);

        void accept();

        std::string to_string();

        virtual ~Proxy();

    private:
        quic::FlowContext *_context;
        net::ipv4::TcpListener *_tcp_listener;

        worker::Looper _accept_looper;
    };

} // proxy

#endif //MULTIPATH_PROXY_PROXY_H
