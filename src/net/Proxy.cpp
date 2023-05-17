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
#include "Bond.h"
#include "../handler/FlowHandler.h"

namespace net {

    Proxy::Proxy(net::ipv4::SockAddr_In sockAddrIn) : listener_(net::ipv4::TcpListener::make(sockAddrIn))
    {
#ifdef __linux__
        listener_->SetSockOpt(IPPROTO_IP, IP_TRANSPARENT, 1);
#endif
        LOG(INFO) << "Proxy(" << sockAddrIn.ToString() << ")";
    }

    Proxy *Proxy::make(net::ipv4::SockAddr_In sockAddrIn) {
        return new Proxy(sockAddrIn);
    }

    Proxy::~Proxy() {
        delete listener_;
    }

    void Proxy::Accept(collections::FlowMap *flows, net::Bond *bond) {
        short events = listener_->Poll(POLLIN, 100); // TODO timeout?

        if (events & POLLIN) {
            // accept incoming connection and get source SockAddr
            net::ipv4::SockAddr_In source;
            net::ipv4::TcpConnection *tcpConnection = listener_->Accept(source);

            // get destination SockAddr
            ipv4::SockAddr_In destination = tcpConnection->GetSockName();

            // create new Flow for connection
            net::Flow *flow = net::Flow::make(source, destination, tcpConnection, flows, bond);

            // add Flow to FlowMap
            flows->Insert(source, destination, flow);

            LOG(INFO) << "Accepted connection " << source.ToString() << "|" << destination.ToString() << ".";
        }
    }

} // net