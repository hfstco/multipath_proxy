//
// Created by Matthias Hofstätter on 04.04.23.
//

#include "args/Args.h"
#include "net/TcpListener.h"
#include "net/TcpConnection.h"
#include "net/SockAddr.h"
#include "net/Bond.h"
#include "net/Proxy.h"
#include "collections/FlowMap.h"
#include "task/ThreadPool.h"
#include "context/Context.h"

int main(int argc, char *argv[]) {
    // init logging
    google::SetLogDestination(0,"mpp.log");
    google::InitGoogleLogging("log_test");
    FLAGS_alsologtostderr = 1;

    // init arguments
    args::init(argc, argv);

    auto ter = net::ipv4::SockAddr_In(args::TER);
    auto sat = net::ipv4::SockAddr_In(args::SAT);

    try {
        // init mpp
        if (args::MODE == args::mode::REMOTE) {
            // create listeners
            net::ipv4::TcpListener *terListener = net::ipv4::TcpListener::make(
                    net::ipv4::SockAddr_In(ter.ip(), ter.port()));
            net::ipv4::TcpListener *satListener = net::ipv4::TcpListener::make(
                    net::ipv4::SockAddr_In(sat.ip(), sat.port()));
            //base::Remote remote = base::Remote(pTcpListenerTer, pTcpListenerSat);

            net::ipv4::TcpConnection *terConnection = terListener->Accept();
            net::ipv4::TcpConnection *satConnection = satListener->Accept();

            // metrics
            context::Context::GetDefaultContext().metrics()->addConnection(terConnection->fd());
            context::Context::GetDefaultContext().metrics()->addConnection(satConnection->fd());

            // create Bond
            net::Bond bond = net::Bond(terConnection, satConnection, &context::Context::GetDefaultContext());

            while(true) {
                sleep(30);
            }

            context::Context::GetDefaultContext().metrics()->removeConnection(terConnection->fd());
            context::Context::GetDefaultContext().metrics()->removeConnection(satConnection->fd());
        } else {
            // create connections
            net::ipv4::TcpConnection *terConnection = net::ipv4::TcpConnection::make(
                    net::ipv4::SockAddr_In(ter.ip(), ter.port()));
            net::ipv4::TcpConnection *satConnection = net::ipv4::TcpConnection::make(
                    net::ipv4::SockAddr_In(sat.ip(), sat.port()));

            // metrics
            context::Context::GetDefaultContext().metrics()->addConnection(terConnection->fd());
            context::Context::GetDefaultContext().metrics()->addConnection(satConnection->fd());

            // create Bond
            net::Bond bond = net::Bond(terConnection, satConnection, &context::Context::GetDefaultContext());

            // create Proxy
            net::Proxy proxy = net::Proxy(net::ipv4::SockAddr_In("172.30.10.2:5000"), &bond, &context::Context::GetDefaultContext());

            while(true) {
                sleep(30);
            }

            //context::Context::GetDefaultContext().metrics()->removeConnection(terConnection->fd());
            //context::Context::GetDefaultContext().metrics()->removeConnection(satConnection->fd());
        }
    } catch (Exception e) {
        LOG(ERROR) << e.what();
    }

    return 0;
}