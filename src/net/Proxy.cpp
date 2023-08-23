//
// Created by Matthias Hofstätter on 17.04.23.
//

#include <glog/logging.h>
#include <future>

#include "Proxy.h"

#include "SockAddr.h"
#include "TcpConnection.h"
#include "TcpListener.h"
#include "Flow.h"
#include "QuicConnection.h"
#include "TER.h"
#include "SAT.h"
#include "QuicStream.h"
#include "../packet/FlowHeader.h"

namespace net {

    Proxy::Proxy(net::ipv4::SockAddr_In sock_addr_in) : _tcp_listener(net::ipv4::TcpListener::make(sock_addr_in)), // create tcp listener on sock_addr_in
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
            ipv4::SockAddr_In destination = tcp_connection->GetSockName();

            // create Flow
            Flow *flow = Flow::make(source, destination, tcp_connection);
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

} // net