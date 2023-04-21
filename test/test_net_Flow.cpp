//
// Created by Matthias Hofstätter on 19.04.23.
//

#include <glog/logging.h>
#include "gtest/gtest.h"
#include <thread>

#include "../src/net/base/TcpConnection.h"
#include "../src/net/base/TcpListener.h"
#include "../src/net/Flow.h"

TEST(net_Flow, Flow_read) {
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
        } catch (SocketException e) {
            LOG(ERROR) << e.what();
            FAIL();
        } catch (NetworkException e) {
            LOG(ERROR) << e.what();
            FAIL();
        } catch (SockAddrException e) {
            LOG(ERROR) << e.what();
            FAIL();
        }
    });

    sleep(5);

    net::ipv4::TcpConnection *tcpConnection = net::ipv4::TcpConnection::make(net::ipv4::SockAddr_In("127.0.0.1", 2350));
    net::Flow *flow = net::Flow::make(tcpConnection);

    std::vector<unsigned char> vector1 = {'h', 'e', 'l', 'l', 'o', 'w', 'o', 'r', 'l', 'd', '!'};
    vector1.resize(vector1.size());
    std::vector<unsigned char> vector2 = std::vector<unsigned char>();
    packet::FlowPacket *flowPacket = nullptr;
    do {
        while (!(flowPacket = flow->ReadFromFlow())) {}

        vector2.insert(vector2.end(), flowPacket->data(), flowPacket->data() + flowPacket->header()->size());
    } while (flowPacket->header()->ctrl() != packet::header::FLOW_CTRL::CLOSE);

    ASSERT_EQ(0, memcmp(vector1.data(), vector2.data(), vector1.size()));

    t1.join();
    delete flow;
}