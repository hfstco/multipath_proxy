//
// Created by Matthias Hofstätter on 08.05.23.
//

#ifndef MULTIPATH_PROXY_PROXYHANDLER_H
#define MULTIPATH_PROXY_PROXYHANDLER_H

#include <atomic>
#include <thread>

namespace net {
    class Proxy;
    class Bond;
}

namespace collections {
    class FlowMap;
}

namespace handler {

    class ProxyHandler {
    public:
        ProxyHandler() = delete;

        static ProxyHandler *make(net::Proxy *proxy, collections::FlowMap *flows, net::Bond *bond);

        void join();

        virtual ~ProxyHandler();

    protected:
        ProxyHandler(net::Proxy *proxy, collections::FlowMap *flows, net::Bond *bond);

    private:
        net::Proxy *proxy_;

        net::Bond *bond_;
        collections::FlowMap *flows_;

        std::atomic_bool stopAcceptHandler_; // TODO implement Handler
        std::thread *acceptHandler_; // TODO implement Handler

        void Accept();
    };

} // handler

#endif //MULTIPATH_PROXY_PROXYHANDLER_H
