//
// Created by Matthias Hofstätter on 14.04.23.
//

#include <glog/logging.h>
#include "gtest/gtest.h"
#include "../src/packet/Packet.h"
#include "../src/packet/HeartBeatPacket.h"
#include "../src/packet/FlowPacket.h"
#include "../src/net/base/SockAddr.h"

TEST(packet_Packet, verify_HeartBeatPacket) {
    packet::HeartBeatPacket *pHeartBeatPacket1 = packet::HeartBeatPacket::make();

    // magic
    EXPECT_EQ(0, memcmp(pHeartBeatPacket1->header(), &packet::header::MAGIC, sizeof(packet::header::MAGIC)));
    // type
    EXPECT_EQ(pHeartBeatPacket1->header()->type(), packet::header::TYPE::HEARTBEAT);
    // size
    EXPECT_EQ(pHeartBeatPacket1->size(), sizeof(packet::header::HeartBeatHeader));
}

TEST(packet_Packet, verify_FlowPacket) {
    net::ipv4::SockAddr_In sourceSockAddrIn = net::ipv4::SockAddr_In("127.0.0.1", 7777);
    net::ipv4::SockAddr_In destinationSockAddrIn = net::ipv4::SockAddr_In("127.0.0.1", 7778);
    uint16_t id = 1;
    uint16_t size = 77;

    packet::header::FlowHeader flowHeader = packet::header::FlowHeader(sourceSockAddrIn, destinationSockAddrIn, id);
    packet::FlowPacket *pFlowPacket = packet::FlowPacket::make(flowHeader, size);

    // header
    EXPECT_TRUE(memcmp(&flowHeader, pFlowPacket->header(), sizeof(packet::header::FlowHeader)));
    // size
    EXPECT_EQ(pFlowPacket->size(), size + sizeof(packet::header::FlowHeader));

    delete pFlowPacket;
}

TEST(packet_Packet, verify_FlowInitPacket) {
    net::ipv4::SockAddr_In sourceSockAddrIn = net::ipv4::SockAddr_In("127.0.0.1", 7777);
    net::ipv4::SockAddr_In destinationSockAddrIn = net::ipv4::SockAddr_In("127.0.0.1", 7778);

    packet::header::FlowInitHeader flowInitHeader(sourceSockAddrIn, destinationSockAddrIn);
    packet::FlowPacket *pFlowInitPacket = packet::FlowPacket::make(flowInitHeader);

    // magic
    EXPECT_EQ(0, memcmp(pFlowInitPacket->header(), &packet::header::MAGIC, sizeof(packet::header::MAGIC)));
    // type
    EXPECT_EQ(pFlowInitPacket->header()->type(), packet::header::TYPE::FLOW);
    // ctrl
    EXPECT_EQ(pFlowInitPacket->header()->ctrl(), packet::header::FLOW_CTRL::INIT);
    // sourceIp
    EXPECT_TRUE(pFlowInitPacket->header()->sourceIp().s_addr == sourceSockAddrIn.sin_addr.s_addr);
    // destinationip
    EXPECT_TRUE(pFlowInitPacket->header()->destinationIp().s_addr == destinationSockAddrIn.sin_addr.s_addr);
    // sourcePort
    EXPECT_EQ(pFlowInitPacket->header()->sourcePort(), sourceSockAddrIn.sin_port);
    // destinationPort
    EXPECT_EQ(pFlowInitPacket->header()->destinationPort(), destinationSockAddrIn.sin_port);
    // id
    EXPECT_EQ(pFlowInitPacket->header()->id(), 0);
    // size
    EXPECT_EQ(pFlowInitPacket->header()->size(), 0);
    // buffer size
    EXPECT_EQ(pFlowInitPacket->size(), sizeof(packet::header::FlowHeader));

    delete pFlowInitPacket;
}

TEST(packet_Packet, verify_FlowClosePacket) {
    net::ipv4::SockAddr_In sourceSockAddrIn = net::ipv4::SockAddr_In("127.0.0.1", 7777);
    net::ipv4::SockAddr_In destinationSockAddrIn = net::ipv4::SockAddr_In("127.0.0.1", 7778);
    uint16_t id = 1;

    packet::header::FlowCloseHeader flowCloseHeader(sourceSockAddrIn, destinationSockAddrIn, id);
    packet::FlowPacket *pFlowClosePacket = packet::FlowPacket::make(flowCloseHeader);

    // magic
    EXPECT_EQ(0, memcmp(pFlowClosePacket->header(), &packet::header::MAGIC, sizeof(packet::header::MAGIC)));
    // type
    EXPECT_EQ(pFlowClosePacket->header()->type(), packet::header::TYPE::FLOW);
    // ctrl
    EXPECT_EQ(pFlowClosePacket->header()->ctrl(), packet::header::FLOW_CTRL::CLOSE);
    // sourceIp
    EXPECT_TRUE(pFlowClosePacket->header()->sourceIp().s_addr == sourceSockAddrIn.sin_addr.s_addr);
    // destinationip
    EXPECT_TRUE(pFlowClosePacket->header()->destinationIp().s_addr == destinationSockAddrIn.sin_addr.s_addr);
    // sourcePort
    EXPECT_EQ(pFlowClosePacket->header()->sourcePort(), sourceSockAddrIn.sin_port);
    // destinationPort
    EXPECT_EQ(pFlowClosePacket->header()->destinationPort(), destinationSockAddrIn.sin_port);
    // id
    EXPECT_EQ(pFlowClosePacket->header()->id(), id);
    // size
    EXPECT_EQ(pFlowClosePacket->header()->size(), 0);
    // buffer size
    EXPECT_EQ(pFlowClosePacket->size(), sizeof(packet::header::FlowHeader));

    delete pFlowClosePacket;
}

TEST(packet_Header, verify_FlowHeader_size) {
    net::ipv4::SockAddr_In sourceSockAddrIn = net::ipv4::SockAddr_In("127.0.0.1", 7777);
    net::ipv4::SockAddr_In destinationSockAddrIn = net::ipv4::SockAddr_In("127.0.0.1", 7778);
    uint16_t id = 7;
    uint16_t size = 77;

    packet::header::FlowHeader flowHeader = packet::header::FlowHeader(sourceSockAddrIn, destinationSockAddrIn, id);

    packet::FlowPacket *pFlowPacket = packet::FlowPacket::make(flowHeader, size);
    // size
    EXPECT_EQ(pFlowPacket->header()->size(), size);
    delete pFlowPacket;

    pFlowPacket = packet::FlowPacket::make(flowHeader);
    // size
    EXPECT_EQ(pFlowPacket->header()->size(), 0);
    delete pFlowPacket;
}

TEST(packet_Packet, Packet_data) {
    packet::Buffer *b1 = packet::Buffer::make(0);
    EXPECT_EQ(b1->size(), 0);


    packet::Buffer *b2 = packet::Packet::make(1);

    packet::Packet *p1 = (packet::Packet *)b2;

    std::vector<unsigned char> d1({'a', 'a', 'a', 'a'});
    net::ipv4::SockAddr_In sa1 = net::ipv4::SockAddr_In("127.0.0.1", 2345);
    net::ipv4::SockAddr_In sa2 = net::ipv4::SockAddr_In("127.0.0.1", 2346);
    packet::header::FlowHeader fh1 = packet::header::FlowHeader(sa1, sa2, 999);

    packet::FlowPacket *fp1 = packet::FlowPacket::make(fh1, d1.data(), d1.size());

    packet::Buffer *b4 = fp1;

    packet::FlowPacket *fp2 = dynamic_cast<packet::FlowPacket *>(b4);
}

/*TEST(packet_Packet, FlowPacket_resize) {
    std::vector<unsigned char> d1({'a', 'a', 'a', 'a'});
    std::vector<unsigned char> d2({'b', 'b', 'b', 'b', 'b', 'b', 'b'});
    net::ipv4::SockAddr_In sa1 = net::ipv4::SockAddr_In("127.0.0.1", 2345);
    net::ipv4::SockAddr_In sa2 = net::ipv4::SockAddr_In("127.0.0.1", 2346);
    packet::FlowHeader fh1 = packet::FlowHeader(sa1, sa2, 0);

    packet::FlowPacket fp1 = packet::FlowPacket(fh1, d1.data(), d1.size());
    EXPECT_EQ(fp1.Header()->Size(), d1.size());
    EXPECT_EQ(0, memcmp(fp1.Data(), d1.data(), fp1.Header()->Size()));
}*/

/*TEST(packet_Packet, FlowPacket_fill) {
    std::vector<unsigned char> d2({'b', 'b', 'b', 'b', 'b', 'b', 'b'});
    net::ipv4::SockAddr_In sa1 = net::ipv4::SockAddr_In("127.0.0.1", 2345);
    net::ipv4::SockAddr_In sa2 = net::ipv4::SockAddr_In("127.0.0.1", 2346);
    packet::FlowHeader fh1 = packet::FlowHeader(sa1, sa2, 1);

    packet::FlowPacket fp1 = packet::FlowPacket(fh1, 1024);
    EXPECT_EQ(fp1.Header()->Size(), 1024);
}*/
