//
// Created by Matthias Hofstätter on 17.04.23.
//

#include <glog/logging.h>
#include "gtest/gtest.h"
#include "../src/packet/FlowHeader.h"
#include "../src/packet/Packet.h"
#include "../src/packet/FlowPacket.h"
#include "../src/collections/FlowPacketQueue.h"

TEST(packet_FlowPacketQueue, FlowPacketQueue_empty) {
    collections::FlowPacketQueue flowPacketQueue1 = collections::FlowPacketQueue();

    ASSERT_TRUE(flowPacketQueue1.empty());

    net::ipv4::SockAddr_In sockAddr1 = net::ipv4::SockAddr_In("127.0.0.1", 2345);
    net::ipv4::SockAddr_In sockAddr2 = net::ipv4::SockAddr_In("127.0.0.1", 2346);
    packet::FlowInitHeader flowInitHeader1 = packet::FlowInitHeader(sockAddr1, sockAddr2);
    packet::FlowPacket *pFlowPacket1 = packet::FlowPacket::make(flowInitHeader1);

    flowPacketQueue1.push(pFlowPacket1);

    ASSERT_FALSE(flowPacketQueue1.empty());
}

TEST(packet_FlowPacketQueue, FlowPacketQueue_pop_on_empty_queue) {
    collections::FlowPacketQueue flowPacketQueue1 = collections::FlowPacketQueue();

    packet::FlowPacket *pNullPtr = flowPacketQueue1.pop();

    ASSERT_EQ(nullptr, pNullPtr);

    net::ipv4::SockAddr_In sockAddr1 = net::ipv4::SockAddr_In("127.0.0.1", 2345);
    net::ipv4::SockAddr_In sockAddr2 = net::ipv4::SockAddr_In("127.0.0.1", 2346);
    packet::FlowInitHeader flowInitHeader1 = packet::FlowInitHeader(sockAddr1, sockAddr2);
    packet::FlowPacket *pFlowPacket1 = packet::FlowPacket::make(flowInitHeader1);

    flowPacketQueue1.push(pFlowPacket1);

    packet::FlowPacket *pFlowPacket11 = flowPacketQueue1.pop();

    ASSERT_EQ(pFlowPacket1, pFlowPacket11);
}

TEST(packet_FlowPacketQueue, FlowPacketQueue_push_and_pop_single_packet) {
    collections::FlowPacketQueue flowPacketQueue1 = collections::FlowPacketQueue();

    net::ipv4::SockAddr_In sockAddr1 = net::ipv4::SockAddr_In("127.0.0.1", 2345);
    net::ipv4::SockAddr_In sockAddr2 = net::ipv4::SockAddr_In("127.0.0.1", 2346);
    packet::FlowInitHeader flowInitHeader1 = packet::FlowInitHeader(sockAddr1, sockAddr2);
    packet::FlowPacket *pFlowPacket1 = packet::FlowPacket::make(flowInitHeader1);

    flowPacketQueue1.push(pFlowPacket1);

    packet::FlowPacket *pFlowPacket2 = flowPacketQueue1.pop();

    ASSERT_EQ(pFlowPacket1, pFlowPacket2);
}

TEST(packet_FlowPacketQueue, FlowPacketQueue_push_and_pop_sorted) { // TODO put packets in array
    collections::FlowPacketQueue flowPacketQueue1 = collections::FlowPacketQueue();

    std::vector<unsigned char> vector1({'a', 'a'});
    std::vector<unsigned char> vector2({'b', 'b', 'b'});
    std::vector<unsigned char> vector3({'c', 'c', 'c', 'c'});
    net::ipv4::SockAddr_In sockAddrIn1 = net::ipv4::SockAddr_In("127.0.0.1", 2345);
    net::ipv4::SockAddr_In sockAddrIn2 = net::ipv4::SockAddr_In("127.0.0.1", 2346);

    packet::FlowInitHeader flowInitHeader1 = packet::FlowInitHeader(sockAddrIn1, sockAddrIn2);
    packet::FlowPacket *pFlowPacket1 = packet::FlowPacket::make(flowInitHeader1);

    packet::FlowHeader flowHeader1 = packet::FlowHeader(sockAddrIn1, sockAddrIn2, 1);
    packet::FlowPacket *pFlowPacket2 = packet::FlowPacket::make(flowHeader1, vector1.data(), vector1.size());

    packet::FlowHeader flowHeader2 = packet::FlowHeader(sockAddrIn1, sockAddrIn2, 2);
    packet::FlowPacket *pFlowPacket3 = packet::FlowPacket::make(flowHeader2, vector2.data(), vector2.size());

    packet::FlowHeader flowHeader3 = packet::FlowHeader(sockAddrIn1, sockAddrIn2, 3);
    packet::FlowPacket *pFlowPacket4 = packet::FlowPacket::make(flowHeader3, vector3.data(), vector3.size());

    packet::FlowCloseHeader flowCloseHeader1 = packet::FlowCloseHeader(sockAddrIn1, sockAddrIn2, 4);
    packet::FlowPacket *pFlowPacket5 = packet::FlowPacket::make(flowCloseHeader1);

    flowPacketQueue1.push(pFlowPacket1);
    flowPacketQueue1.push(pFlowPacket2);
    flowPacketQueue1.push(pFlowPacket3);
    flowPacketQueue1.push(pFlowPacket4);
    flowPacketQueue1.push(pFlowPacket5);

    packet::FlowPacket *pFlowPacket11 = flowPacketQueue1.pop();
    ASSERT_EQ(pFlowPacket1, pFlowPacket11);
    packet::FlowPacket *pFlowPacket12 = flowPacketQueue1.pop();
    ASSERT_EQ(pFlowPacket2, pFlowPacket12);
    packet::FlowPacket *pFlowPacket13 = flowPacketQueue1.pop();
    ASSERT_EQ(pFlowPacket3, pFlowPacket13);
    packet::FlowPacket *pFlowPacket14 = flowPacketQueue1.pop();
    ASSERT_EQ(pFlowPacket4, pFlowPacket14);
    packet::FlowPacket *pFlowPacket15 = flowPacketQueue1.pop();
    ASSERT_EQ(pFlowPacket5, pFlowPacket15);
}

TEST(packet_FlowPacketQueue, FlowPacketQueue_push_pop_unsorted) {
    collections::FlowPacketQueue flowPacketQueue1 = collections::FlowPacketQueue();

    std::vector<unsigned char> vector1({'a', 'a'});
    std::vector<unsigned char> vector2({'b', 'b', 'b'});
    std::vector<unsigned char> vector3({'c', 'c', 'c', 'c'});
    net::ipv4::SockAddr_In sockAddrIn1 = net::ipv4::SockAddr_In("127.0.0.1", 2345);
    net::ipv4::SockAddr_In sockAddrIn2 = net::ipv4::SockAddr_In("127.0.0.1", 2346);

    packet::FlowInitHeader flowInitHeader1 = packet::FlowInitHeader(sockAddrIn1, sockAddrIn2);
    packet::FlowPacket *pFlowPacket1 = packet::FlowPacket::make(flowInitHeader1);

    packet::FlowHeader flowHeader1 = packet::FlowHeader(sockAddrIn1, sockAddrIn2, 1);
    packet::FlowPacket *pFlowPacket2 = packet::FlowPacket::make(flowHeader1, vector1.data(), vector1.size());

    packet::FlowHeader flowHeader2 = packet::FlowHeader(sockAddrIn1, sockAddrIn2, 2);
    packet::FlowPacket *pFlowPacket3 = packet::FlowPacket::make(flowHeader2, vector2.data(), vector2.size());

    packet::FlowHeader flowHeader3 = packet::FlowHeader(sockAddrIn1, sockAddrIn2, 3);
    packet::FlowPacket *pFlowPacket4 = packet::FlowPacket::make(flowHeader3, vector3.data(), vector3.size());

    packet::FlowCloseHeader flowCloseHeader1 = packet::FlowCloseHeader(sockAddrIn1, sockAddrIn2, 4);
    packet::FlowPacket *pFlowPacket5 = packet::FlowPacket::make(flowCloseHeader1);

    flowPacketQueue1.push(pFlowPacket1);
    flowPacketQueue1.push(pFlowPacket4);
    flowPacketQueue1.push(pFlowPacket2);
    flowPacketQueue1.push(pFlowPacket5);
    flowPacketQueue1.push(pFlowPacket3);

    packet::FlowPacket *pFlowPacket11 = flowPacketQueue1.pop();
    ASSERT_EQ(pFlowPacket1, pFlowPacket11);
    packet::FlowPacket *pFlowPacket12 = flowPacketQueue1.pop();
    ASSERT_EQ(pFlowPacket2, pFlowPacket12);
    packet::FlowPacket *pFlowPacket13 = flowPacketQueue1.pop();
    ASSERT_EQ(pFlowPacket3, pFlowPacket13);
    packet::FlowPacket *pFlowPacket14 = flowPacketQueue1.pop();
    ASSERT_EQ(pFlowPacket4, pFlowPacket14);
    packet::FlowPacket *pFlowPacket15 = flowPacketQueue1.pop();
    ASSERT_EQ(pFlowPacket5, pFlowPacket15);
}

TEST(packet_FlowPacketQueue, FlowPacketQueue_id_not_available) {
    collections::FlowPacketQueue flowPacketQueue1 = collections::FlowPacketQueue();

    std::vector<unsigned char> vector1({'a', 'a'});
    std::vector<unsigned char> vector2({'b', 'b', 'b'});
    std::vector<unsigned char> vector3({'c', 'c', 'c', 'c'});
    net::ipv4::SockAddr_In sockAddrIn1 = net::ipv4::SockAddr_In("127.0.0.1", 2345);
    net::ipv4::SockAddr_In sockAddrIn2 = net::ipv4::SockAddr_In("127.0.0.1", 2346);

    packet::FlowInitHeader flowInitHeader1 = packet::FlowInitHeader(sockAddrIn1, sockAddrIn2);
    packet::FlowPacket *pFlowPacket1 = packet::FlowPacket::make(flowInitHeader1);

    packet::FlowHeader flowHeader1 = packet::FlowHeader(sockAddrIn1, sockAddrIn2, 1);
    packet::FlowPacket *pFlowPacket2 = packet::FlowPacket::make(flowHeader1, vector1.data(), vector1.size());

    packet::FlowHeader flowHeader2 = packet::FlowHeader(sockAddrIn1, sockAddrIn2, 2);
    packet::FlowPacket *pFlowPacket3 = packet::FlowPacket::make(flowHeader2, vector2.data(), vector2.size());

    packet::FlowHeader flowHeader3 = packet::FlowHeader(sockAddrIn1, sockAddrIn2, 3);
    packet::FlowPacket *pFlowPacket4 = packet::FlowPacket::make(flowHeader3, vector3.data(), vector3.size());

    packet::FlowCloseHeader flowCloseHeader1 = packet::FlowCloseHeader(sockAddrIn1, sockAddrIn2, 4);
    packet::FlowPacket *pFlowPacket5 = packet::FlowPacket::make(flowCloseHeader1);

    flowPacketQueue1.push(pFlowPacket1);
    flowPacketQueue1.push(pFlowPacket4);
    flowPacketQueue1.push(pFlowPacket2);
    flowPacketQueue1.push(pFlowPacket5);

    packet::FlowPacket *pFlowPacket11 = flowPacketQueue1.pop();
    ASSERT_EQ(pFlowPacket1, pFlowPacket11);
    packet::FlowPacket *pFlowPacket12 = flowPacketQueue1.pop();
    ASSERT_EQ(pFlowPacket2, pFlowPacket12);
    packet::FlowPacket *pNullPtr = flowPacketQueue1.pop();
    ASSERT_EQ(nullptr, pNullPtr);

    flowPacketQueue1.push(pFlowPacket3);

    packet::FlowPacket *pFlowPacket13 = flowPacketQueue1.pop();
    ASSERT_EQ(pFlowPacket3, pFlowPacket13);
    packet::FlowPacket *pFlowPacket14 = flowPacketQueue1.pop();
    ASSERT_EQ(pFlowPacket4, pFlowPacket14);
    packet::FlowPacket *pFlowPacket15 = flowPacketQueue1.pop();
    ASSERT_EQ(pFlowPacket5, pFlowPacket15);

}
