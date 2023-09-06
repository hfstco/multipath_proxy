//
// Created by Matthias Hofstätter on 05.03.23.
//

/*#include <thread>
#include <glog/logging.h>
#include "gtest/gtest.h"
#include "../src/net/TcpConnection.h"
#include "../src/net/TcpListener.h"
#include "../src/packet/Buffer.h"

TEST(net_Connection, ipv4_TcpListener) {

}

TEST(net_Connection, ipv4_TcpConnection) {
    std::thread t1([&] {
        try {
            net::ipv4::TcpListener *l1 = net::ipv4::TcpListener::make(net::ipv4::SockAddr_In("127.0.0.1", 2349));
            net::ipv4::TcpConnection *c1 = l1->Accept();

            std::vector<unsigned char> buffer1 = {'h', 'e', 'l', 'l', 'o', 'w', 'o', 'r', 'l', 'd', '!'};
            packet::Buffer *b1 = packet::Buffer::make(buffer1);
            c1->Send(b1->data(), b1->size(), 0);

            delete l1;
            delete c1;
            delete b1;
        } catch (Exception e) {
            LOG(ERROR) << e.what();
            FAIL();
        }
    });

    sleep(1);

    try {
        net::ipv4::TcpConnection *c2 = net::ipv4::TcpConnection::make(net::ipv4::SockAddr_In("127.0.0.1", 2349));

        packet::Buffer *b1 = packet::Buffer::make(1024);
        c2->Recv(b1->data(), b1->size(), 0);

        std::vector<unsigned char> buffer2 = {'h', 'e', 'l', 'l', 'o', 'w', 'o', 'r', 'l', 'd', '!'};

        EXPECT_EQ(0, memcmp(b1->data(), buffer2.data(), buffer2.size()));

        delete c2;
        delete b1;
    } catch (Exception e) {
        LOG(ERROR) << e.what();
        FAIL();
    }

    t1.join();
}*/

/*TEST(net_Connection, ipv6_TcpConnection) {
    std::thread t1([&] {
        try {
            net::ipv6::SockAddr_In6 sockAddrIn6 = net::ipv6::SockAddr_In6("::1", 2350);
            net::ipv6::TcpListener *l1 = net::ipv6::TcpListener::make(sockAddrIn6);
            net::ipv6::TcpConnection *c1 = l1->accept();

            std::vector<unsigned char> buffer1 = {'h', 'e', 'l', 'l', 'o', 'w', 'o', 'r', 'l', 'd', '!'};
            packet::Buffer *b1 = packet::Buffer::make(buffer1);
            c1->Send(b1->data(), b1->size(), 0);

            delete l1;
            delete c1;
            delete b1;
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

    sleep(1);

    try {
        net::ipv6::TcpConnection *c2 = net::ipv6::TcpConnection::make(net::ipv6::SockAddr_In6("::1", 2350));

        packet::Buffer *b1 = packet::Buffer::make(1024);
        c2->Recv(b1->data(), b1->size(), 0);

        std::vector<unsigned char> buffer2 = {'h', 'e', 'l', 'l', 'o', 'w', 'o', 'r', 'l', 'SendToSat', '!'};

        EXPECT_EQ(0, memcmp(b1->data(), buffer2.data(), buffer2.size()));

        delete c2;
        delete b1;
    } catch (SocketErrorException e) {
        LOG(ERROR) << e.what();
        FAIL();
    } catch (NetworkException e) {
        LOG(INFO) << e.what();
        FAIL();
    } catch (SockAddrErrorException e) {
        LOG(ERROR) << e.what();
        FAIL();
    }

    t1.join();
}*/
