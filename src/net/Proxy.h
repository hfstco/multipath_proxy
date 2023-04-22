//
// Created by Matthias Hofstätter on 17.04.23.
//

#ifndef MULTIPATH_PROXY_PROXY_H
#define MULTIPATH_PROXY_PROXY_H

#include "base/TcpListener.h"
#include "../handler/Handler.h"
#include "Flow.h"
#include "../collections/FlowMap.h"
#include "../utils/Utils.h"

namespace net {

    class Proxy {
    public:
        Proxy() = delete;

        static Proxy *make(net::ipv4::SockAddr_In sockAddrIn, collections::FlowMap *flows) {
            return new Proxy(sockAddrIn, flows);
        }

        virtual ~Proxy() {
            stop_.store(true);

            acceptHandler_->join();
            delete acceptHandler_;
        }

    private:
        net::ipv4::TcpListener *listener_;
        collections::FlowMap *flows_;

        std::atomic_bool stop_; // TODO implement Handler
        std::thread *acceptHandler_; // TODO implement Handler

        Proxy(net::ipv4::SockAddr_In sockAddrIn, collections::FlowMap *flows) : listener_(net::ipv4::TcpListener::make(sockAddrIn)), flows_(flows),
                                                                                stop_(false), acceptHandler_(new std::thread(&Proxy::AcceptFromConnection, this)) {}

        void AcceptFromConnection() {
            LOG(INFO) << "Starting AcceptFromConnection loop...";

            while (!stop_.load()) {
                short events = listener_->Poll(POLLIN, 100); // TODO timeout?

                if (events & POLLIN) {
                    net::ipv4::SockAddr_In destinationSockAddrIn;
                    net::ipv4::TcpConnection *pTcpConnection = listener_->Accept(destinationSockAddrIn);
                    net::Flow *pFlow = net::Flow::make(pTcpConnection);

                    flows_->insert(utils::ConnectionString(pTcpConnection->GetSockName().ip(), pTcpConnection->GetSockName().port(), destinationSockAddrIn.ip(), destinationSockAddrIn.port()), pFlow);

                    LOG(INFO) << "Accepted connection " << pTcpConnection->GetPeerName().ip() << ":" << pTcpConnection->GetPeerName().port() << "|" << destinationSockAddrIn.ip() << ":" << destinationSockAddrIn.port() << ".";
                }
            }

            LOG(INFO) << "Stopping AcceptFromConnection loop...";
        }
    };

} // net

#endif //MULTIPATH_PROXY_PROXY_H
