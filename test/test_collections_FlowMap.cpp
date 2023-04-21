//
// Created by Matthias Hofstätter on 17.04.23.
//

#include <glog/logging.h>
#include "gtest/gtest.h"
#include "../src/collections/FlowMap.h"
#include "../src/net/Proxy.h"

TEST(collections_FlowMap, insert) {
    collections::FlowMap flowMap1 = collections::FlowMap();

    net::Proxy *proxy1 = net::Proxy::make(net::ipv4::SockAddr_In("127.0.0.1", 5000), &flowMap1);

    net::ipv4::TcpConnection *pTcpConnection1 = net::ipv4::TcpConnection::make(net::ipv4::SockAddr_In("127.0.0.1", 5000));
    net::ipv4::TcpConnection *pTcpConnection2 = net::ipv4::TcpConnection::make(net::ipv4::SockAddr_In("127.0.0.1", 5000));
    net::ipv4::TcpConnection *pTcpConnection3 = net::ipv4::TcpConnection::make(net::ipv4::SockAddr_In("127.0.0.1", 5000));

    sleep(1);

    // TODO
    EXPECT_EQ(flowMap1.size(), 3);

    delete proxy1;
    delete pTcpConnection1;
    delete pTcpConnection2;
    delete pTcpConnection3;
}