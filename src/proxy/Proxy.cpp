//
// Created by Matthias Hofstätter on 17.04.23.
//

#include <glog/logging.h>
#include <future>

#include "Proxy.h"

#include "../net/TcpConnection.h"
#include "../net/TcpListener.h"
#include "../flow/Flow.h"
#include "../quic/FlowContext.h"
#include "../threadpool/Threadpool.h"

namespace proxy {

    Proxy::Proxy(quic::FlowContext *context, net::ipv4::SockAddr_In sock_addr_in) : _context(context), // quic context
                                                                                    _listener(net::ipv4::TcpListener::make(sock_addr_in)) { // create tcp listener on sock_addr_in
#ifdef __linux__ // for debugging on win or macos, tproxy is only available on unix distributions
        _listener->SetSockOpt(IPPROTO_IP, IP_TRANSPARENT, 1);
#endif

        THREADPOOL.push_task(&Proxy::accept, this);

        VLOG(1) << "Proxy(" << sock_addr_in.to_string() << ") * " << to_string();
        LOG(INFO) << "Proxy listening on " << sock_addr_in.to_string() << "...";
    }

    void Proxy::accept() {
        try {
            // no data available
            if ((_listener->Poll(POLLIN, 0) & POLLIN) == 0) {
                THREADPOOL.push_task(&Proxy::accept, this);
                return;
            }

            // accept incoming connection and get source SockAddr
            //net::ipv4::SockAddr_In source;
            net::ipv4::TcpConnection *connection = _listener->Accept();

            // get destination SockAddr
            // net::ipv4::SockAddr_In destination = connection->GetSockName();

            // create Flow
            flow::Flow *flow = _context->new_flow(connection);

            // queue accept()
            THREADPOOL.push_task(&Proxy::accept, this);
        } catch (Exception e) {
            LOG(ERROR) << e.what();
        }
    }

    std::string Proxy::to_string() {
        return "Proxy[fd=" + std::to_string(_listener->fd()) + ", sockAddr=" + _listener->GetSockName().to_string() + "]";
    }

    Proxy::~Proxy() {
        DLOG(INFO) << to_string() << ".~Proxy()";

        delete _listener;
    }

} // proxy