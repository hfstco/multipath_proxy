//
// Created by Matthias Hofstätter on 17.04.23.
//

#include <thread>
#include <glog/logging.h>
#include <sys/poll.h>

#include "Proxy.h"

#include "base/SockAddr.h"
#include "base/TcpConnection.h"
#include "base/TcpListener.h"
#include "Flow.h"
#include "../collections/FlowMap.h"

namespace net {

    Proxy::Proxy(net::ipv4::SockAddr_In sockAddrIn, collections::FlowMap *flows, net::Bond *pBond) : listener_(net::ipv4::TcpListener::make(sockAddrIn)), flows_(flows), bond_(pBond),
    stop_(false), acceptHandler_(new std::thread(&Proxy::AcceptFromConnection, this)) {
#ifdef __linux__
        listener_->SetSockOpt(IPPROTO_IP, IP_TRANSPARENT, 1);
#endif
    }

    Proxy *Proxy::make(net::ipv4::SockAddr_In sockAddrIn, collections::FlowMap *pFlows, net::Bond *pBond) {
        return new Proxy(sockAddrIn, pFlows, pBond);
    }

    Proxy::~Proxy() {
        stop_.store(true);

        acceptHandler_->join();
        delete acceptHandler_;
    }

    void Proxy::AcceptFromConnection() {
        LOG(INFO) << "Starting AcceptFromConnection loop...";

        while (!stop_.load()) {
            short events = listener_->Poll(POLLIN, 100); // TODO timeout?

            if (events & POLLIN) {
                net::ipv4::SockAddr_In destinationSockAddrIn;
                net::ipv4::TcpConnection *pTcpConnection = listener_->Accept(destinationSockAddrIn);
                net::Flow *pFlow = net::Flow::make(pTcpConnection, bond_);

                flows_->insert(utils::ConnectionString(pTcpConnection->GetSockName().ip(), pTcpConnection->GetSockName().port(), destinationSockAddrIn.ip(), destinationSockAddrIn.port()), pFlow);

                LOG(INFO) << "Accepted connection " << pTcpConnection->GetPeerName().ip() << ":" << pTcpConnection->GetPeerName().port() << "|" << destinationSockAddrIn.ip() << ":" << destinationSockAddrIn.port() << ".";
            }
        }

        LOG(INFO) << "Stopping AcceptFromConnection loop...";
    }

} // net