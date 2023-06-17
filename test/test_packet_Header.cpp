//
// Created by Matthias Hofstätter on 21.04.23.
//

#include <glog/logging.h>
#include <gtest/gtest.h>
#include "../src/packet/Header.h"
#include "../src/packet/FlowHeader.h"
#include "../src/packet/HeartBeatHeader.h"
#include "../src/net/SockAddr.h"

TEST(packet_Header, verify_FlowHeader) {
    net::ipv4::SockAddr_In sourceSockAddrIn = net::ipv4::SockAddr_In("127.0.0.1", 7777);
    net::ipv4::SockAddr_In destinationSockAddrIn = net::ipv4::SockAddr_In("127.0.0.1", 7778);
    uint16_t id = 7;

    packet::FlowHeader flowHeader = packet::FlowHeader(sourceSockAddrIn, destinationSockAddrIn, id);

    // magic
    EXPECT_EQ(0, memcmp(&flowHeader, &packet::MAGIC, sizeof(packet::MAGIC)));
    // type
    EXPECT_EQ(flowHeader.type(), packet::TYPE::FLOW);
    // ctrl
    EXPECT_EQ(flowHeader.ctrl(), packet::FLOW_CTRL::REGULAR);
    // sourceIp
    EXPECT_TRUE(flowHeader.sourceIp().s_addr == sourceSockAddrIn.sin_addr.s_addr);
    // destinationip
    EXPECT_TRUE(flowHeader.destinationIp().s_addr == destinationSockAddrIn.sin_addr.s_addr);
    // sourcePort
    EXPECT_EQ(flowHeader.sourcePort(), sourceSockAddrIn.sin_port);
    // destinationPort
    EXPECT_EQ(flowHeader.destinationPort(), destinationSockAddrIn.sin_port);
    // id
    EXPECT_EQ(flowHeader.id(), id);
    // size
    EXPECT_EQ(flowHeader.size(), 0);
}

