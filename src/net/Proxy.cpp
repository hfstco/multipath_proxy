//
// Created by Matthias Hofstätter on 17.04.23.
//

#include <thread>
#include <glog/logging.h>
#include <sys/poll.h>
#include <future>
#include <assert.h>

#include "Proxy.h"

#include "SockAddr.h"
#include "TcpConnection.h"
#include "TcpListener.h"
#include "Flow.h"
#include "../collections/FlowMap.h"
#include "Bond.h"
#include "../task/ThreadPool.h"
#include "../context/Context.h"
#include "../metrics/Metrics.h"

namespace net {

    Proxy::Proxy(net::ipv4::SockAddr_In sockAddrIn, net::Bond *bond, context::Context *context) : listener_(net::ipv4::TcpListener::make(sockAddrIn)), bond_(bond), context_(context),
                                                                                                  acceptLooper_(std::bind(&Proxy::Accept, this)) {
#ifdef __linux__
        listener_->SetSockOpt(IPPROTO_IP, IP_TRANSPARENT, 1);
#endif
        acceptLooper_.Start();

        DLOG(INFO) << "Proxy(" << sockAddrIn.ToString() << ") * " << ToString();
    }

    void Proxy::Accept() {
        try {
            // accept incoming connection and get source SockAddr
            net::ipv4::SockAddr_In source;
            net::ipv4::TcpConnection *tcpConnection = listener_->Accept(source); // blocking here

            // get destination SockAddr
            ipv4::SockAddr_In destination = tcpConnection->GetSockName();

            // metrics
            //context_->metrics()->addConnection(tcpConnection->fd());

            // create new Flow for connection
            net::Flow *flow = net::Flow::make(source, destination, tcpConnection, bond_, context_);

            // add Flow to FlowMap
            {
                std::lock_guard lock(context_->flows()->mutex());

                context_->flows()->Insert(source, destination, flow);
            }
        } catch (Exception e) {
            LOG(ERROR) << e.what();
        }
    }

    std::string Proxy::ToString() {
        return "Proxy[fd=" + std::to_string(listener_->fd()) + ", sockAddr=" + listener_->GetSockName().ToString() + "]";
    }

    Proxy::~Proxy() {
        DLOG(INFO) << ToString() << ".~Proxy()";

        delete listener_;
    }

} // net