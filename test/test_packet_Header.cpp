//
// Created by Matthias Hofstätter on 21.04.23.
//

#include <glog/logging.h>
#include <gtest/gtest.h>
#include "../src/packet/header/Header.h"
#include "../src/packet/header/FlowHeader.h"
#include "../src/packet/header/HeartBeatHeader.h"

TEST(packet_Header, verify_FlowHeader) {
    net::ipv4::SockAddr_In sourceSockAddrIn = net::ipv4::SockAddr_In("127.0.0.1", 7777);
    net::ipv4::SockAddr_In destinationSockAddrIn = net::ipv4::SockAddr_In("127.0.0.1", 7778);
    uint16_t id = 7;

    packet::header::FlowHeader flowHeader = packet::header::FlowHeader(sourceSockAddrIn, destinationSockAddrIn, id);

    // magic
    EXPECT_EQ(0, memcmp(&flowHeader, &packet::header::MAGIC, sizeof(packet::header::MAGIC)));
    // type
    EXPECT_EQ(flowHeader.type(), packet::header::TYPE::FLOW);
    // ctrl
    EXPECT_EQ(flowHeader.ctrl(), packet::header::FLOW_CTRL::REGULAR);
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

TEST(packet_Header, verify_FlowInitHeader) {
    net::ipv4::SockAddr_In sourceSockAddrIn = net::ipv4::SockAddr_In("127.0.0.1", 7777);
    net::ipv4::SockAddr_In destinationSockAddrIn = net::ipv4::SockAddr_In("127.0.0.1", 7778);

    packet::header::FlowInitHeader flowInitHeader(sourceSockAddrIn, destinationSockAddrIn);

    // magic
    EXPECT_EQ(0, memcmp(&flowInitHeader, &packet::header::MAGIC, sizeof(packet::header::MAGIC)));
    // type
    EXPECT_EQ(flowInitHeader.type(), packet::header::TYPE::FLOW);
    // ctrl
    EXPECT_EQ(flowInitHeader.ctrl(), packet::header::FLOW_CTRL::INIT);
    // sourceIp
    EXPECT_TRUE(flowInitHeader.sourceIp().s_addr == sourceSockAddrIn.sin_addr.s_addr);
    // destinationip
    EXPECT_TRUE(flowInitHeader.destinationIp().s_addr == destinationSockAddrIn.sin_addr.s_addr);
    // sourcePort
    EXPECT_EQ(flowInitHeader.sourcePort(), sourceSockAddrIn.sin_port);
    // destinationPort
    EXPECT_EQ(flowInitHeader.destinationPort(), destinationSockAddrIn.sin_port);
    // id
    EXPECT_EQ(flowInitHeader.id(), 0);
    // size
    EXPECT_EQ(flowInitHeader.size(), 0);
}

TEST(packet_Header, verify_FlowCloseHeader) {
    net::ipv4::SockAddr_In sourceSockAddrIn = net::ipv4::SockAddr_In("127.0.0.1", 7777);
    net::ipv4::SockAddr_In destinationSockAddrIn = net::ipv4::SockAddr_In("127.0.0.1", 7778);
    uint16_t id = 7;

    packet::header::FlowCloseHeader flowCloseHeader(sourceSockAddrIn, destinationSockAddrIn, id);

    // magic
    EXPECT_EQ(0, memcmp(&flowCloseHeader, &packet::header::MAGIC, sizeof(packet::header::MAGIC)));
    // type
    EXPECT_EQ(flowCloseHeader.type(), packet::header::TYPE::FLOW);
    // ctrl
    EXPECT_EQ(flowCloseHeader.ctrl(), packet::header::FLOW_CTRL::CLOSE);
    // sourceIp
    EXPECT_TRUE(flowCloseHeader.sourceIp().s_addr == sourceSockAddrIn.sin_addr.s_addr);
    // destinationip
    EXPECT_TRUE(flowCloseHeader.destinationIp().s_addr == destinationSockAddrIn.sin_addr.s_addr);
    // sourcePort
    EXPECT_EQ(flowCloseHeader.sourcePort(), sourceSockAddrIn.sin_port);
    // destinationPort
    EXPECT_EQ(flowCloseHeader.destinationPort(), destinationSockAddrIn.sin_port);
    // id
    EXPECT_EQ(flowCloseHeader.id(), id);
    // size
    EXPECT_EQ(flowCloseHeader.size(), 0);
}
