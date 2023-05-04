//
// Created by Matthias Hofstätter on 04.04.23.
//

#include "base/Local.h"
#include "base/Remote.h"
#include "args/Args.h"
#include "net/base/TcpListener.h"
#include "net/base/TcpConnection.h"
#include "Endpoint.h"

int main(int argc, char *argv[]) {
    // init logging
    google::SetLogDestination(0,"mpp.log");
    google::InitGoogleLogging("log_test");
    FLAGS_alsologtostderr = 1;

    // init arguments
    args::init(argc, argv);

    auto ter = Endpoint(args::TER);
    auto sat = Endpoint(args::SAT);

    try {
        // init mpp
        if (args::MODE == args::mode::REMOTE) {
            net::ipv4::TcpListener *pTcpListenerTer = net::ipv4::TcpListener::make(
                    net::ipv4::SockAddr_In(ter.ip(), ter.port()));
            net::ipv4::TcpListener *pTcpListenerSat = net::ipv4::TcpListener::make(
                    net::ipv4::SockAddr_In(sat.ip(), sat.port()));
            base::Remote remote = base::Remote(pTcpListenerTer, pTcpListenerSat);
        } else {
            net::ipv4::TcpConnection *pTcpConnectionTer = net::ipv4::TcpConnection::make(
                    net::ipv4::SockAddr_In(ter.ip(), ter.port()));
            net::ipv4::TcpConnection *pTcpConnectionSat = net::ipv4::TcpConnection::make(
                    net::ipv4::SockAddr_In(sat.ip(), sat.port()));
            base::Local local = base::Local(pTcpConnectionTer, pTcpConnectionSat,
                                            net::ipv4::SockAddr_In("129.168.10.2", 5000));
        }
    } catch (Exception e) {
        LOG(ERROR) << e.what();
    }

    while(true) {}

    return 0;
}