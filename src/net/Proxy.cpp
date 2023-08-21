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

namespace net {

    Proxy::Proxy(net::ipv4::SockAddr_In sockAddrIn) : _tcpListener(net::ipv4::TcpListener::make(sockAddrIn)), // create tcp listener on sockAddrIn
                                                                                                          _TER(TER), _SAT(SAT), // TER & SAT quic connections
                                                                                                          _acceptLooper([this] { Accept(); }) { // thread which runs Accept() in a loop
#ifdef __linux__ // for debugging on win or macos, tproxy is only available on unix distributions
        _tcpListener->SetSockOpt(IPPROTO_IP, IP_TRANSPARENT, 1);
#endif
        _acceptLooper.Start();

        DLOG(INFO) << "Proxy(" << sockAddrIn.ToString() << ") * " << ToString();
    }

    void Proxy::Accept() {
        try {
            // accept incoming connection and get source SockAddr
            net::ipv4::SockAddr_In source;
            net::ipv4::TcpConnection *tcpConnection = _tcpListener->Accept(source); // blocking here

            // get destination SockAddr
            ipv4::SockAddr_In destination = tcpConnection->GetSockName();

            // create Flow
            Flow *flow = Flow::make(source, destination, tcpConnection);
            // create Streams for Flow
            // TODO check if streams + 1 nullptr
            QuicStream *terQuicStream = TER->CreateNewStream(false, flow);
            flow->_terTxStream = terQuicStream;
            QuicStream *satQuicStream = SAT->CreateNewStream(false, flow);
            flow->_satTxStream = satQuicStream;
            flow->MakeActiveFlow();
        } catch (Exception e) {
            LOG(ERROR) << e.what();
        }
    }

    std::string Proxy::ToString() {
        return "Proxy[fd=" + std::to_string(_tcpListener->fd()) + ", sockAddr=" + _tcpListener->GetSockName().ToString() + "]";
    }

    Proxy::~Proxy() {
        DLOG(INFO) << ToString() << ".~Proxy()";

        delete _tcpListener;
    }

} // net