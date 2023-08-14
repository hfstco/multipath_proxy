//
// Created by Matthias Hofstätter on 04.04.23.
//

#include <iostream>
#include <netinet/tcp.h>

#include "args/ARGS.h"
#include "net/TcpListener.h"
#include "net/TcpConnection.h"
#include "net/SockAddr.h"
#include "net/Proxy.h"
#include "net/QuicServerConnection.h"
#include "net/QuicClientConnection.h"
#include "net/QuicConnection.h"
#include "net/TER.h"
#include "net/SAT.h"


int main(int argc, char *argv[]) {
    // init logging
    google::SetLogDestination(0,"mpp.log");
    google::InitGoogleLogging("log_test");
    FLAGS_alsologtostderr = true;

    // init arguments
    args::init(argc, argv);

    if (args::MODE == args::mode::LOCAL) { // local
        if(args::TER_ENABLED) {
            net::ipv4::SockAddr_In terSockAddr = net::ipv4::SockAddr_In(args::TER_STRING);
            TER = new net::QuicClientConnection(terSockAddr.ip(), terSockAddr.port(), false, "ter_ticket_store.bin", "ter_token_store.bin");
        }
        if(args::SAT_ENABLED) {
            net::ipv4::SockAddr_In satSockAddr = net::ipv4::SockAddr_In(args::SAT_STRING);
            SAT = new net::QuicClientConnection(satSockAddr.ip(), satSockAddr.port(), true, "sat_ticket_store.bin", "sat_token_store.bin");
        }

        net::Proxy proxy = net::Proxy(net::ipv4::SockAddr_In(args::PROXY_STRING));

        //exit when key pressed
        std::cin.ignore();
    } else { // remote
        if(args::TER_ENABLED) {
            net::ipv4::SockAddr_In terSockAddr = net::ipv4::SockAddr_In(args::TER_STRING);
            TER = new net::QuicServerConnection(terSockAddr.port(), false);
        }
        if(args::SAT_ENABLED) {
            net::ipv4::SockAddr_In satSockAddr = net::ipv4::SockAddr_In(args::SAT_STRING);
            SAT = new net::QuicServerConnection(satSockAddr.port(), true);
        }

        //exit when key pressed
        std::cin.ignore();
    }

    if(args::TER_ENABLED) {
        delete TER;
    }
    if(args::SAT_ENABLED) {
        delete SAT;
    }

    return 0;
}