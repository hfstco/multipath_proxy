//
// Created by Matthias Hofstätter on 21.04.23.
//

#include <glog/logging.h>
#include <gtest/gtest.h>
#include "../src/packet/Header.h"
#include "../src/packet/FlowHeader.h"
#include "../src/packet/HeartBeatHeader.h"

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

TEST(packet_Header, verify_FlowInitHeader) {
    net::ipv4::SockAddr_In sourceSockAddrIn = net::ipv4::SockAddr_In("127.0.0.1", 7777);
    net::ipv4::SockAddr_In destinationSockAddrIn = net::ipv4::SockAddr_In("127.0.0.1", 7778);

    packet::FlowInitHeader flowInitHeader(sourceSockAddrIn, destinationSockAddrIn);

    // magic
    EXPECT_EQ(0, memcmp(&flowInitHeader, &packet::MAGIC, sizeof(packet::MAGIC)));
    // type
    EXPECT_EQ(flowInitHeader.type(), packet::TYPE::FLOW);
    // ctrl
    EXPECT_EQ(flowInitHeader.ctrl(), packet::FLOW_CTRL::INIT);
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

    packet::FlowCloseHeader flowCloseHeader(sourceSockAddrIn, destinationSockAddrIn, id);

    // magic
    EXPECT_EQ(0, memcmp(&flowCloseHeader, &packet::MAGIC, sizeof(packet::MAGIC)));
    // type
    EXPECT_EQ(flowCloseHeader.type(), packet::TYPE::FLOW);
    // ctrl
    EXPECT_EQ(flowCloseHeader.ctrl(), packet::FLOW_CTRL::CLOSE);
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

