//
// Created by Matthias Hofstätter on 14.04.23.
//

#include <glog/logging.h>
#include "gtest/gtest.h"
#include "../src/packet/Packet.h"
#include "../src/packet/HeartBeatPacket.h"
#include "../src/packet/HeartBeatHeader.h"
#include "../src/packet/FlowHeader.h"
#include "../src/packet/FlowPacket.h"
#include "../src/net/base/SockAddr.h"
#include "../src/packet/Header.h"

TEST(packet_Packet, verify_HeartBeatPacket) {
    packet::HeartBeatPacket *pHeartBeatPacket1 = packet::HeartBeatPacket::make();

    // magic
    EXPECT_EQ(0, memcmp(pHeartBeatPacket1->header(), &packet::MAGIC, sizeof(packet::MAGIC)));
    // type
    EXPECT_EQ(pHeartBeatPacket1->header()->type(), packet::TYPE::HEARTBEAT);
    // size
    EXPECT_EQ(pHeartBeatPacket1->size(), sizeof(packet::HeartBeatHeader));
}

TEST(packet_Packet, verify_FlowPacket) {
    net::ipv4::SockAddr_In sourceSockAddrIn = net::ipv4::SockAddr_In("127.0.0.1", 7777);
    net::ipv4::SockAddr_In destinationSockAddrIn = net::ipv4::SockAddr_In("127.0.0.1", 7778);
    uint16_t id = 1;
    uint16_t size = 77;

    packet::FlowHeader flowHeader = packet::FlowHeader(sourceSockAddrIn, destinationSockAddrIn, id);
    packet::FlowPacket *pFlowPacket = packet::FlowPacket::make(flowHeader, size);

    // header
    EXPECT_TRUE(memcmp(&flowHeader, pFlowPacket->header(), sizeof(packet::FlowHeader)));
    // size
    EXPECT_EQ(pFlowPacket->size(), size + sizeof(packet::FlowHeader));

    delete pFlowPacket;
}

TEST(packet_Header, verify_FlowHeader_size) {
    net::ipv4::SockAddr_In sourceSockAddrIn = net::ipv4::SockAddr_In("127.0.0.1", 7777);
    net::ipv4::SockAddr_In destinationSockAddrIn = net::ipv4::SockAddr_In("127.0.0.1", 7778);
    uint16_t id = 7;
    uint16_t size = 77;

    packet::FlowHeader flowHeader = packet::FlowHeader(sourceSockAddrIn, destinationSockAddrIn, id);

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
    packet::FlowHeader fh1 = packet::FlowHeader(sa1, sa2, 999);

    packet::FlowPacket *fp1 = packet::FlowPacket::make(fh1, d1.data(), d1.size());

    packet::Buffer *b4 = fp1;

    packet::FlowPacket *fp2 = (packet::FlowPacket *)b4;
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
