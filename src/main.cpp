//
// Created by Matthias Hofstätter on 04.04.23.
//

#include <iostream>
#include <netinet/tcp.h>

#include "args/Args.h"
#include "net/TcpListener.h"
#include "net/TcpConnection.h"
#include "net/SockAddr.h"
#include "net/Bond.h"
#include "net/Proxy.h"
#include "collections/FlowMap.h"
#include "context/Context.h"


int main(int argc, char *argv[]) {
    // init logging
    google::SetLogDestination(0,"mpp.log");
    google::InitGoogleLogging("log_test");
    FLAGS_alsologtostderr = true;

    // init arguments
    args::init(argc, argv);

    net::ipv4::SockAddr_In ter;
    net::ipv4::SockAddr_In sat;
    if(args::TER_ENABLED) {
        ter = net::ipv4::SockAddr_In(args::TER);
    }
    if(args::SAT_ENABLED) {
        sat = net::ipv4::SockAddr_In(args::SAT);
    }

    net::ipv4::TcpConnection *terConnection = nullptr;
    net::ipv4::TcpConnection *satConnection = nullptr;

    context::Context *context = &context::Context::GetDefaultContext();

    try {
        // init mpp
        if (args::MODE == args::mode::REMOTE) {
            // create ter & sat listeners
            if(args::TER_ENABLED) {
                net::ipv4::TcpListener *terListener = net::ipv4::TcpListener::make(
                        net::ipv4::SockAddr_In(ter.ip(), ter.port()));
                terConnection = terListener->Accept();
                // enable no delay option
                terConnection->SetSockOpt(IPPROTO_TCP, TCP_NODELAY, 1);
            }
            if(args::SAT_ENABLED) {
                net::ipv4::TcpListener *satListener = net::ipv4::TcpListener::make(
                        net::ipv4::SockAddr_In(sat.ip(), sat.port()));
                satConnection = satListener->Accept();
                // enable no delay option
                satConnection->SetSockOpt(IPPROTO_TCP, TCP_NODELAY, 1);
            }

            // create Bond
            net::Bond bond = net::Bond(terConnection, satConnection, context);

            //exit when key pressed
            std::cin.ignore();
        } else {
            // create ter & sat connections
            if(args::TER_ENABLED) {
                terConnection = net::ipv4::TcpConnection::make(
                        net::ipv4::SockAddr_In(ter.ip(), ter.port()));
                // enable no delay option
                terConnection->SetSockOpt(IPPROTO_TCP, TCP_NODELAY, 1);
            }
            if(args::SAT_ENABLED) {
                satConnection = net::ipv4::TcpConnection::make(
                        net::ipv4::SockAddr_In(sat.ip(), sat.port()));
                // enable no delay option
                satConnection->SetSockOpt(IPPROTO_TCP, TCP_NODELAY, 1);
            }

            // create Bond
            net::Bond bond = net::Bond(terConnection, satConnection, &context::Context::GetDefaultContext());

            // create Proxy
            net::Proxy proxy = net::Proxy(net::ipv4::SockAddr_In(args::PROXY), &bond, context);

            //exit when key pressed
            std::cin.ignore();
        }
    } catch (Exception e) {
        LOG(ERROR) << e.what();
    }

    delete terConnection;
    delete satConnection;

    return 0;
}