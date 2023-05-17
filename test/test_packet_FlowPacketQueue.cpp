//
// Created by Matthias Hofstätter on 17.04.23.
//

#include <glog/logging.h>
#include "gtest/gtest.h"
#include "../src/packet/FlowHeader.h"
#include "../src/packet/Packet.h"
#include "../src/packet/FlowPacket.h"
#include "../src/collections/FlowPacketQueue.h"
#include "../src/net/base/SockAddr.h"

TEST(packet_FlowPacketQueue, FlowPacketQueue_empty) {
    collections::FlowPacketQueue flowPacketQueue1 = collections::FlowPacketQueue();

    ASSERT_TRUE(flowPacketQueue1.Empty());

    net::ipv4::SockAddr_In sockAddr1 = net::ipv4::SockAddr_In("127.0.0.1", 2345);
    net::ipv4::SockAddr_In sockAddr2 = net::ipv4::SockAddr_In("127.0.0.1", 2346);
    packet::FlowHeader flowHeader1 = packet::FlowHeader(sockAddr1, sockAddr2, 0);
    packet::FlowPacket *pFlowPacket1 = packet::FlowPacket::make(flowHeader1);

    flowPacketQueue1.Push(pFlowPacket1);

    ASSERT_FALSE(flowPacketQueue1.Empty());
}

TEST(packet_FlowPacketQueue, FlowPacketQueue_pop_on_empty_queue) {
    collections::FlowPacketQueue flowPacketQueue1 = collections::FlowPacketQueue();

    packet::FlowPacket *pNullPtr = flowPacketQueue1.Pop();

    ASSERT_EQ(nullptr, pNullPtr);

    net::ipv4::SockAddr_In sockAddr1 = net::ipv4::SockAddr_In("127.0.0.1", 2345);
    net::ipv4::SockAddr_In sockAddr2 = net::ipv4::SockAddr_In("127.0.0.1", 2346);
    packet::FlowHeader flowInitHeader1 = packet::FlowHeader(sockAddr1, sockAddr2, 0);
    packet::FlowPacket *pFlowPacket1 = packet::FlowPacket::make(flowInitHeader1);

    flowPacketQueue1.Push(pFlowPacket1);

    packet::FlowPacket *pFlowPacket11 = flowPacketQueue1.Pop();

    ASSERT_EQ(pFlowPacket1, pFlowPacket11);
}

TEST(packet_FlowPacketQueue, FlowPacketQueue_push_and_pop_single_packet) {
    collections::FlowPacketQueue flowPacketQueue1 = collections::FlowPacketQueue();

    net::ipv4::SockAddr_In sockAddr1 = net::ipv4::SockAddr_In("127.0.0.1", 2345);
    net::ipv4::SockAddr_In sockAddr2 = net::ipv4::SockAddr_In("127.0.0.1", 2346);
    packet::FlowHeader flowInitHeader1 = packet::FlowHeader(sockAddr1, sockAddr2, 0);
    packet::FlowPacket *pFlowPacket1 = packet::FlowPacket::make(flowInitHeader1);

    flowPacketQueue1.Push(pFlowPacket1);

    packet::FlowPacket *pFlowPacket2 = flowPacketQueue1.Pop();

    ASSERT_EQ(pFlowPacket1, pFlowPacket2);
}

TEST(packet_FlowPacketQueue, FlowPacketQueue_push_and_pop_sorted) { // TODO put packets in array
    collections::FlowPacketQueue flowPacketQueue1 = collections::FlowPacketQueue();

    std::vector<unsigned char> vector1({'a', 'a'});
    std::vector<unsigned char> vector2({'b', 'b', 'b'});
    std::vector<unsigned char> vector3({'c', 'c', 'c', 'c'});
    net::ipv4::SockAddr_In sockAddrIn1 = net::ipv4::SockAddr_In("127.0.0.1", 2345);
    net::ipv4::SockAddr_In sockAddrIn2 = net::ipv4::SockAddr_In("127.0.0.1", 2346);


    packet::FlowHeader flowHeader1 = packet::FlowHeader(sockAddrIn1, sockAddrIn2, 0);
    packet::FlowPacket *pFlowPacket1 = packet::FlowPacket::make(flowHeader1, vector1.data(), vector1.size());

    packet::FlowHeader flowHeader2 = packet::FlowHeader(sockAddrIn1, sockAddrIn2, 1);
    packet::FlowPacket *pFlowPacket2 = packet::FlowPacket::make(flowHeader2, vector2.data(), vector2.size());

    packet::FlowHeader flowHeader3 = packet::FlowHeader(sockAddrIn1, sockAddrIn2, 2);
    packet::FlowPacket *pFlowPacket3 = packet::FlowPacket::make(flowHeader3, vector3.data(), vector3.size());

    packet::FlowHeader flowHeader4 = packet::FlowHeader(sockAddrIn1, sockAddrIn2, 3);
    packet::FlowPacket *pFlowPacket4 = packet::FlowPacket::make(flowHeader4);

    flowPacketQueue1.Push(pFlowPacket1);
    flowPacketQueue1.Push(pFlowPacket2);
    flowPacketQueue1.Push(pFlowPacket3);
    flowPacketQueue1.Push(pFlowPacket4);

    packet::FlowPacket *pFlowPacket11 = flowPacketQueue1.Pop();
    ASSERT_EQ(pFlowPacket1, pFlowPacket11);
    packet::FlowPacket *pFlowPacket12 = flowPacketQueue1.Pop();
    ASSERT_EQ(pFlowPacket2, pFlowPacket12);
    packet::FlowPacket *pFlowPacket13 = flowPacketQueue1.Pop();
    ASSERT_EQ(pFlowPacket3, pFlowPacket13);
    packet::FlowPacket *pFlowPacket14 = flowPacketQueue1.Pop();
    ASSERT_EQ(pFlowPacket4, pFlowPacket14);
}

TEST(packet_FlowPacketQueue, FlowPacketQueue_push_pop_unsorted) {
    collections::FlowPacketQueue flowPacketQueue1 = collections::FlowPacketQueue();

    std::vector<unsigned char> vector1({'a', 'a'});
    std::vector<unsigned char> vector2({'b', 'b', 'b'});
    std::vector<unsigned char> vector3({'c', 'c', 'c', 'c'});
    net::ipv4::SockAddr_In sockAddrIn1 = net::ipv4::SockAddr_In("127.0.0.1", 2345);
    net::ipv4::SockAddr_In sockAddrIn2 = net::ipv4::SockAddr_In("127.0.0.1", 2346);

    packet::FlowHeader flowHeader1 = packet::FlowHeader(sockAddrIn1, sockAddrIn2, 0);
    packet::FlowPacket *pFlowPacket1 = packet::FlowPacket::make(flowHeader1, vector1.data(), vector1.size());

    packet::FlowHeader flowHeader2 = packet::FlowHeader(sockAddrIn1, sockAddrIn2, 1);
    packet::FlowPacket *pFlowPacket2 = packet::FlowPacket::make(flowHeader2, vector2.data(), vector2.size());

    packet::FlowHeader flowHeader3 = packet::FlowHeader(sockAddrIn1, sockAddrIn2, 2);
    packet::FlowPacket *pFlowPacket3 = packet::FlowPacket::make(flowHeader3, vector3.data(), vector3.size());

    packet::FlowHeader flowHeader4 = packet::FlowHeader(sockAddrIn1, sockAddrIn2, 3);
    packet::FlowPacket *pFlowPacket4 = packet::FlowPacket::make(flowHeader4);

    flowPacketQueue1.Push(pFlowPacket1);
    flowPacketQueue1.Push(pFlowPacket4);
    flowPacketQueue1.Push(pFlowPacket3);
    flowPacketQueue1.Push(pFlowPacket2);

    packet::FlowPacket *pFlowPacket11 = flowPacketQueue1.Pop();
    ASSERT_EQ(pFlowPacket1, pFlowPacket11);
    packet::FlowPacket *pFlowPacket12 = flowPacketQueue1.Pop();
    ASSERT_EQ(pFlowPacket2, pFlowPacket12);
    packet::FlowPacket *pFlowPacket13 = flowPacketQueue1.Pop();
    ASSERT_EQ(pFlowPacket3, pFlowPacket13);
    packet::FlowPacket *pFlowPacket14 = flowPacketQueue1.Pop();
    ASSERT_EQ(pFlowPacket4, pFlowPacket14);
}

TEST(packet_FlowPacketQueue, FlowPacketQueue_id_not_available) {
    collections::FlowPacketQueue flowPacketQueue1 = collections::FlowPacketQueue();

    std::vector<unsigned char> vector1({'a', 'a'});
    std::vector<unsigned char> vector2({'b', 'b', 'b'});
    std::vector<unsigned char> vector3({'c', 'c', 'c', 'c'});
    net::ipv4::SockAddr_In sockAddrIn1 = net::ipv4::SockAddr_In("127.0.0.1", 2345);
    net::ipv4::SockAddr_In sockAddrIn2 = net::ipv4::SockAddr_In("127.0.0.1", 2346);

    packet::FlowHeader flowHeader1 = packet::FlowHeader(sockAddrIn1, sockAddrIn2, 0);
    packet::FlowPacket *pFlowPacket1 = packet::FlowPacket::make(flowHeader1, vector1.data(), vector1.size());

    packet::FlowHeader flowHeader2 = packet::FlowHeader(sockAddrIn1, sockAddrIn2, 1);
    packet::FlowPacket *pFlowPacket2 = packet::FlowPacket::make(flowHeader2, vector2.data(), vector2.size());

    packet::FlowHeader flowHeader3 = packet::FlowHeader(sockAddrIn1, sockAddrIn2, 2);
    packet::FlowPacket *pFlowPacket3 = packet::FlowPacket::make(flowHeader3, vector3.data(), vector3.size());

    packet::FlowHeader flowHeader4 = packet::FlowHeader(sockAddrIn1, sockAddrIn2, 3);
    packet::FlowPacket *pFlowPacket4 = packet::FlowPacket::make(flowHeader4);

    flowPacketQueue1.Push(pFlowPacket1);
    flowPacketQueue1.Push(pFlowPacket4);
    flowPacketQueue1.Push(pFlowPacket2);

    packet::FlowPacket *pFlowPacket11 = flowPacketQueue1.Pop();
    ASSERT_EQ(pFlowPacket1, pFlowPacket11);
    packet::FlowPacket *pFlowPacket12 = flowPacketQueue1.Pop();
    ASSERT_EQ(pFlowPacket2, pFlowPacket12);
    packet::FlowPacket *pNullPtr = flowPacketQueue1.Pop();
    ASSERT_EQ(nullptr, pNullPtr);

    flowPacketQueue1.Push(pFlowPacket3);

    packet::FlowPacket *pFlowPacket13 = flowPacketQueue1.Pop();
    ASSERT_EQ(pFlowPacket3, pFlowPacket13);
    packet::FlowPacket *pFlowPacket14 = flowPacketQueue1.Pop();
    ASSERT_EQ(pFlowPacket4, pFlowPacket14);

}
