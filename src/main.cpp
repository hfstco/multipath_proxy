//
// Created by Matthias Hofstätter on 04.04.23.
//

#include "base/Local.h"
#include "base/Remote.h"
#include "args/Args.h"
#include "log/Log.h"

int main(int argc, char *argv[]) {
    // init logging
    log::init();

    // init arguments
    args::init(argc, argv);

    // init mpp
    if(args::MODE == args::mode::REMOTE) {
        net::ipv4::TcpListener *pTcpListenerTer = net::ipv4::TcpListener(new net::ipv4::SockAddr_In("192.168.20.1", 6000));
        net::ipv4::TcpListener *pTcpListenerSat = net::ipv4::TcpListener(new net::ipv4::SockAddr_In("192.168.30.1", 7000))
        base::Remote remote = base::Remote(pTcpListenerTer, pTcpListenerSat);
    } else {
        net::ipv4::TcpConnection *pTcpConnectionTer = net::ipv4::TcpConnection(new net::ipv4::SockAddr_In("192.168.20.1", 6000));
        net::ipv4::TcpConnection *pTcpConnectionSat = net::ipv4::TcpConnection(new net::ipv4::SockAddr_In("192.168.30.1", 7000));
        base::Local local = base::Local(pTcpConnectionTer, pTcpConnectionSat);
    }

    while(true) {}

    return 0;
}