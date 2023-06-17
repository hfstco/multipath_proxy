//
// Created by Matthias Hofstätter on 19.04.23.
//

#include <glog/logging.h>
#include "gtest/gtest.h"
#include <thread>

#include "../src/net/TcpConnection.h"
#include "../src/net/TcpListener.h"
#include "../src/net/Flow.h"
#include "../src/packet/Buffer.h"
#include "../src/packet/FlowHeader.h"
#include "../src/packet/FlowPacket.h"

/*TEST(net_Flow, read_from_connection) {
    std::thread t1([&] {
        try {
            net::ipv4::TcpListener *tcpListener1 = net::ipv4::TcpListener::make(net::ipv4::SockAddr_In("127.0.0.1", 2350));
            net::ipv4::TcpConnection *tcpConnection1 = tcpListener1->Accept();
            delete tcpListener1;

            std::vector<unsigned char> vector1 = {'h', 'e', 'l', 'l', 'o', 'w', 'o', 'r', 'l', 'd', '!'};

            packet::Buffer *buffer1 = packet::Buffer::make(vector1.data(), 2);
            tcpConnection1->Send(buffer1->data(), buffer1->size(), 0);
            delete buffer1;

            sleep(2);

            packet::Buffer *buffer2 = packet::Buffer::make(vector1.data() + 2, 2);
            tcpConnection1->Send(buffer2->data(), buffer2->size(), 0);
            delete buffer2;

            packet::Buffer *buffer3 = packet::Buffer::make(vector1.data() + 4, 2);
            tcpConnection1->Send(buffer3->data(), buffer3->size(), 0);
            delete buffer3;

            sleep(4);

            packet::Buffer *buffer4 = packet::Buffer::make(vector1.data() + 6, 2);
            tcpConnection1->Send(buffer4->data(), buffer4->size(), 0);
            delete buffer4;

            packet::Buffer *buffer5 = packet::Buffer::make(vector1.data() + 8, 2);
            tcpConnection1->Send(buffer5->data(), buffer5->size(), 0);
            delete buffer5;

            packet::Buffer *buffer6 = packet::Buffer::make(vector1.data() + 10, 1);
            tcpConnection1->Send(buffer6->data(), buffer6->size(), 0);
            delete buffer6;

            delete tcpConnection1;
        } catch (SocketErrorException e) {
            LOG(ERROR) << e.what();
            FAIL();
        } catch (NetworkException e) {
            LOG(ERROR) << e.what();
            FAIL();
        } catch (SockAddrErrorException e) {
            LOG(ERROR) << e.what();
            FAIL();
        }
    });

    sleep(5);

    net::ipv4::TcpConnection *tcpConnection = net::ipv4::TcpConnection::make(net::ipv4::SockAddr_In("127.0.0.1", 2350));
    net::Flow *flow = net::Flow::make(tcpConnection, nullptr);

    std::vector<unsigned char> vector1 = {'h', 'e', 'l', 'l', 'o', 'w', 'o', 'r', 'l', 'd', '!'};
    vector1.resize(vector1.size());
    std::vector<unsigned char> vector2 = std::vector<unsigned char>();
    packet::FlowPacket *flowPacket = nullptr;
    do {
        while (!(flowPacket = flow->ReadFromFlow())) {}

        vector2.insert(vector2.end(), flowPacket->data(), flowPacket->data() + flowPacket->header()->size());
    } while (flowPacket->header()->ctrl() != packet::FLOW_CTRL::CLOSE);

    ASSERT_EQ(0, memcmp(vector1.data(), vector2.data(), vector1.size()));

    t1.join();
    delete flow;
}*/

/*TEST(net_Flow, write_to_connection_sorted) {
    std::thread t1([&] {
        sleep(1);

        std::vector<unsigned char> vector1 = {'h', 'e', 'l', 'l', 'o', 'w', 'o', 'r', 'l', 'd', '!'};

        net::ipv4::TcpConnection *tcpConnection = net::ipv4::TcpConnection::make(net::ipv4::SockAddr_In("127.0.0.1", 2350));
        net::Flow *flow = net::Flow::make(tcpConnection, nullptr);

        net::ipv4::SockAddr_In sourceSockAddrIn = net::ipv4::SockAddr_In("127.0.0.1", 7777);
        net::ipv4::SockAddr_In destinationSockAddrIn = net::ipv4::SockAddr_In("127.0.0.1", 7778);

        packet::FlowHeader flowHeader1 = packet::FlowHeader(sourceSockAddrIn, destinationSockAddrIn, 1);
        packet::FlowPacket *pFlowPacket1 = packet::FlowPacket::make(flowHeader1, vector1.data(), 2);
        packet::FlowHeader flowHeader2 = packet::FlowHeader(sourceSockAddrIn, destinationSockAddrIn, 2);
        packet::FlowPacket *pFlowPacket2 = packet::FlowPacket::make(flowHeader2, vector1.data() + 2, 2);
        packet::FlowHeader flowHeader3 = packet::FlowHeader(sourceSockAddrIn, destinationSockAddrIn, 3);
        packet::FlowPacket *pFlowPacket3 = packet::FlowPacket::make(flowHeader3, vector1.data() + 4, 2);
        packet::FlowHeader flowHeader4 = packet::FlowHeader(sourceSockAddrIn, destinationSockAddrIn, 4);
        packet::FlowPacket *pFlowPacket4 = packet::FlowPacket::make(flowHeader4, vector1.data() + 6, 2);
        packet::FlowHeader flowHeader5 = packet::FlowHeader(sourceSockAddrIn, destinationSockAddrIn, 5);
        packet::FlowPacket *pFlowPacket5 = packet::FlowPacket::make(flowHeader5, vector1.data() + 8, 2);
        packet::FlowHeader flowHeader6 = packet::FlowHeader(sourceSockAddrIn, destinationSockAddrIn, 6);
        packet::FlowPacket *pFlowPacket6 = packet::FlowPacket::make(flowHeader6, vector1.data() + 10, 1);

        packet::FlowHeader flowCloseHeader1 = packet::FlowCloseHeader(sourceSockAddrIn, destinationSockAddrIn, 7);
        packet::FlowPacket *pFlowPacket7 = packet::FlowPacket::make(flowCloseHeader1);

        flow->WriteToFlow(pFlowPacket1);
        flow->WriteToFlow(pFlowPacket2);
        flow->WriteToFlow(pFlowPacket3);
        flow->WriteToFlow(pFlowPacket4);
        flow->WriteToFlow(pFlowPacket5);
        flow->WriteToFlow(pFlowPacket6);
        flow->WriteToFlow(pFlowPacket7);

        // delete flow;
    });

    try {
        net::ipv4::TcpListener *tcpListener1 = net::ipv4::TcpListener::make(net::ipv4::SockAddr_In("127.0.0.1", 2350));
        net::ipv4::TcpConnection *tcpConnection1 = tcpListener1->Accept();
        delete tcpListener1;

        std::vector<unsigned char> vector1 = {'h', 'e', 'l', 'l', 'o', 'w', 'o', 'r', 'l', 'd', '!'};
        std::vector<unsigned char> vector2(11);

        int bytes_read = 0;
        while (bytes_read < vector2.size()) {
            bytes_read += tcpConnection1->Recv(vector2.data() + bytes_read, vector2.size() - bytes_read, 0);
        }

        EXPECT_EQ(0, memcmp(vector2.data(), vector1.data(), vector2.size()));

        delete tcpConnection1;
    } catch (Exception e) {
        LOG(ERROR) << e.what();
        FAIL();
    }

    t1.join();
}*/

/*
TEST(net_Flow, write_to_connection_unsorted) {
    std::thread t1([&] {
        sleep(1);

        std::vector<unsigned char> vector1 = {'h', 'e', 'l', 'l', 'o', 'w', 'o', 'r', 'l', 'd', '!'};

        net::ipv4::TcpConnection *tcpConnection = net::ipv4::TcpConnection::make(net::ipv4::SockAddr_In("127.0.0.1", 2350));
        net::Flow *flow = net::Flow::make(tcpConnection, nullptr);

        net::ipv4::SockAddr_In sourceSockAddrIn = net::ipv4::SockAddr_In("127.0.0.1", 7777);
        net::ipv4::SockAddr_In destinationSockAddrIn = net::ipv4::SockAddr_In("127.0.0.1", 7778);

        packet::FlowHeader flowHeader1 = packet::FlowHeader(sourceSockAddrIn, destinationSockAddrIn, 1);
        packet::FlowPacket *pFlowPacket1 = packet::FlowPacket::make(flowHeader1, vector1.data(), 2);
        packet::FlowHeader flowHeader2 = packet::FlowHeader(sourceSockAddrIn, destinationSockAddrIn, 2);
        packet::FlowPacket *pFlowPacket2 = packet::FlowPacket::make(flowHeader2, vector1.data() + 2, 2);
        packet::FlowHeader flowHeader3 = packet::FlowHeader(sourceSockAddrIn, destinationSockAddrIn, 3);
        packet::FlowPacket *pFlowPacket3 = packet::FlowPacket::make(flowHeader3, vector1.data() + 4, 2);
        packet::FlowHeader flowHeader4 = packet::FlowHeader(sourceSockAddrIn, destinationSockAddrIn, 4);
        packet::FlowPacket *pFlowPacket4 = packet::FlowPacket::make(flowHeader4, vector1.data() + 6, 2);
        packet::FlowHeader flowHeader5 = packet::FlowHeader(sourceSockAddrIn, destinationSockAddrIn, 5);
        packet::FlowPacket *pFlowPacket5 = packet::FlowPacket::make(flowHeader5, vector1.data() + 8, 2);
        packet::FlowHeader flowHeader6 = packet::FlowHeader(sourceSockAddrIn, destinationSockAddrIn, 6);
        packet::FlowPacket *pFlowPacket6 = packet::FlowPacket::make(flowHeader6, vector1.data() + 10, 1);

        packet::FlowHeader flowCloseHeader1 = packet::FlowCloseHeader(sourceSockAddrIn, destinationSockAddrIn, 7);
        packet::FlowPacket *pFlowPacket7 = packet::FlowPacket::make(flowCloseHeader1);

        flow->WriteToFlow(pFlowPacket2);
        flow->WriteToFlow(pFlowPacket1);
        sleep(2);
        flow->WriteToFlow(pFlowPacket4);
        flow->WriteToFlow(pFlowPacket3);
        flow->WriteToFlow(pFlowPacket6);
        sleep(3);
        flow->WriteToFlow(pFlowPacket7);
        flow->WriteToFlow(pFlowPacket5);

        // delete flow;
    });

    try {
        net::ipv4::TcpListener *tcpListener1 = net::ipv4::TcpListener::make(net::ipv4::SockAddr_In("127.0.0.1", 2350));
        net::ipv4::TcpConnection *tcpConnection1 = tcpListener1->Accept();
        delete tcpListener1;

        std::vector<unsigned char> vector1 = {'h', 'e', 'l', 'l', 'o', 'w', 'o', 'r', 'l', 'd', '!'};
        std::vector<unsigned char> vector2(11);

        int bytes_read = 0;
        while (bytes_read < vector2.size()) {
            bytes_read += tcpConnection1->Recv(vector2.data() + bytes_read, vector2.size() - bytes_read, 0);
        }

        EXPECT_EQ(0, memcmp(vector2.data(), vector1.data(), vector2.size()));

        delete tcpConnection1;
    } catch (Exception e) {
        LOG(ERROR) << e.what();
        FAIL();
    }

    t1.join();
}*/
