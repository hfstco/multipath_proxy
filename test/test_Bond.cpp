//
// Created by Matthias Hofstätter on 25.04.23.
//

#include <thread>
#include "gtest/gtest.h"
#include "../src/collections/FlowMap.h"
#include "../src/net/Proxy.h"
#include "../src/net/Bond.h"
#include "../src/net/base/TcpListener.h"
#include "../src/net/base/TcpConnection.h"
#include "../src/net/base/SockAddr.h"

/*TEST(net_Bond, simple) {
    collections::FlowMap flowMapLocal = collections::FlowMap();
    collections::FlowMap flowMapRemote = collections::FlowMap();

    // server
    net::ipv4::TcpListener *pTcpListener1 = net::ipv4::TcpListener::make(net::ipv4::SockAddr_In("127.0.0.1", 8765));

    // remote
    net::ipv4::TcpListener *pTcpListenerRemoteTer = net::ipv4::TcpListener::make(net::ipv4::SockAddr_In("127.0.0.1", 6000));
    net::ipv4::TcpListener *pTcpListenerRemoteSat= net::ipv4::TcpListener::make(net::ipv4::SockAddr_In("127.0.0.1", 6001));

    // local
    net::ipv4::TcpConnection *pTcpConnectionLocalTer = net::ipv4::TcpConnection::make(net::ipv4::SockAddr_In("127.0.0.1", 6000));
    net::ipv4::TcpConnection *pTcpConnectionLocalSat = net::ipv4::TcpConnection::make(net::ipv4::SockAddr_In("127.0.0.1", 6001));

    // accept remote side
    net::ipv4::TcpConnection *pTcpConnectionRemoteTer = pTcpListenerRemoteTer->Accept();
    net::ipv4::TcpConnection *pTcpConnectionRemoteSat = pTcpListenerRemoteSat->Accept();

    // create bond local
    net::Bond *bondLocal = net::Bond::make(pTcpConnectionLocalTer, pTcpConnectionLocalSat, &flowMapLocal);
    // create bond remote
    net::Bond *bondRemote = net::Bond::make(pTcpConnectionRemoteTer, pTcpConnectionRemoteSat, &flowMapRemote);

    // proxy
    net::Proxy *proxy1 = net::Proxy::make(net::ipv4::SockAddr_In("127.0.0.1", 5000), &flowMapLocal, bondLocal);

    // client
    net::ipv4::TcpConnection *pTcpConnectionClient = net::ipv4::TcpConnection::make(net::ipv4::SockAddr_In("127.0.0.1", 5000));

    sleep(1);
}*/
