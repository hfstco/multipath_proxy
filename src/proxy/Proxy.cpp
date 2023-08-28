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

namespace proxy {

    Proxy::Proxy(quic::FlowContext *context, net::ipv4::SockAddr_In sock_addr_in)
            : _context(context), _tcp_listener(net::ipv4::TcpListener::make(sock_addr_in)), // create tcp listener on sock_addr_in
                                                        _accept_looper([this] { accept(); }) { // thread which runs accept() in a loop
#ifdef __linux__ // for debugging on win or macos, tproxy is only available on unix distributions
        _tcp_listener->SetSockOpt(IPPROTO_IP, IP_TRANSPARENT, 1);
#endif
        _accept_looper.start();

        DLOG(INFO) << "Proxy(" << sock_addr_in.to_string() << ") * " << to_string();
    }

    void Proxy::accept() {
        try {
            // accept incoming connection and get source SockAddr
            net::ipv4::SockAddr_In source;
            net::ipv4::TcpConnection *tcp_connection = _tcp_listener->Accept(source); // blocking here

            // get destination SockAddr
            net::ipv4::SockAddr_In destination = tcp_connection->GetSockName();

            // create Flow
            flow::Flow *flow = _context->new_flow(source, destination, tcp_connection);
        } catch (Exception e) {
            LOG(ERROR) << e.what();
        }
    }

    std::string Proxy::to_string() {
        return "Proxy[fd=" + std::to_string(_tcp_listener->fd()) + ", sockAddr=" +
                _tcp_listener->GetSockName().to_string() + "]";
    }

    Proxy::~Proxy() {
        DLOG(INFO) << to_string() << ".~Proxy()";

        delete _tcp_listener;
    }

} // proxy