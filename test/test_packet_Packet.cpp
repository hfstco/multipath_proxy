//
// Created by Matthias Hofstätter on 14.04.23.
//

#include <glog/logging.h>
#include "gtest/gtest.h"
#include "../src/packet/Packet.h"
#include "../src/packet/HeartBeatPacket.h"
#include "../src/packet/FlowPacket.h"
#include "../src/net/base/SockAddr.h"

TEST(packet_Packet, Packet_simple) {
    packet::HeartBeatPacket *hbp1 = packet::HeartBeatPacket::make();

    auto *p1 = (packet::Packet *)hbp1;
    EXPECT_EQ(packet::header::TYPE::HEARTBEAT, p1->header()->type());
}

TEST(packet_Packet, FlowPacket_simple) {
    std::vector<unsigned char> d1({'a', 'a', 'a', 'a'});
    net::ipv4::SockAddr_In sa1 = net::ipv4::SockAddr_In("127.0.0.1", 2345);
    net::ipv4::SockAddr_In sa2 = net::ipv4::SockAddr_In("127.0.0.1", 2346);
    packet::header::FlowHeader fh1 = packet::header::FlowHeader(sa1, sa2, 999);

    packet::FlowPacket *fp1 = packet::FlowPacket::make(fh1);
    EXPECT_EQ(0, memcmp(fp1->header(), &fh1, sizeof(packet::header::FlowHeader)));

    packet::FlowPacket *fp2 = packet::FlowPacket::make(fh1, d1.data(), d1.size());
    EXPECT_EQ(0, memcmp(fp2->data(), d1.data(), sizeof(d1.size())));

    /*EXPECT_EQ(0, memcmp(&sa1.sin_addr, &fp1.header()->sourceIp(), sizeof(in_addr))); // TODO compare in_addr, sockaddr_in
    EXPECT_EQ(0, memcmp(&sa2.sin_addr, &fp1.header()->destinationIp(), sizeof(in_addr)));
    EXPECT_EQ(0, memcmp(&sa1.sin_port, &fp1.header()->sourcePort(), sizeof(unsigned short)));
    EXPECT_EQ(0, memcmp(&sa2.sin_port, &fp1.header()->destinationPort(), sizeof(unsigned short)));*/
    EXPECT_EQ(fp2->header()->id(), 999);
    EXPECT_EQ(d1.size(), fp2->header()->size());
    EXPECT_EQ(fp2->header()->ctrl(), packet::header::FLOW_CTRL::REGULAR);

    packet::header::FlowCloseHeader fch1 = packet::header::FlowCloseHeader(sa1, sa2, 9998);
    packet::FlowPacket *fp3 = packet::FlowPacket::make(fch1);
    EXPECT_EQ(fp3->header()->ctrl(), packet::header::FLOW_CTRL::CLOSE);
    EXPECT_EQ(fp3->header()->size(), 0);
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
