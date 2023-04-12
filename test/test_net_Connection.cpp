//
// Created by Matthias Hofstätter on 05.03.23.
//

#include <thread>
#include <glog/logging.h>

#include "gtest/gtest.h"
#include "../src/net/TcpConnection.h"
#include "../src/net/TcpListener.h"

TEST(net_Connection, ipv4_tcp_connection) {
    std::thread t1([&] {
        try {
            net::ipv4::TcpListener l1 = net::ipv4::TcpListener(net::ipv4::SockAddr_In("127.0.0.1", 2349));
            net::ipv4::TcpConnection c1 = l1.Accept();
            l1.Close();
            std::vector<char> buffer1 = {'h', 'e', 'l', 'l', 'o', 'w', 'o', 'r', 'l', 'd', '!'};
            c1.Send(buffer1, 0);
            c1.Close();
        } catch (SocketError e) {
            LOG(ERROR) << e.what();
            FAIL();
        } catch (NetworkError e) {
            LOG(ERROR) << e.what();
            FAIL();
        } catch (SockAddrError e) {
            LOG(ERROR) << e.what();
            FAIL();
        }
    });

    sleep(1);

    try {
        net::ipv4::TcpConnection c2 = net::ipv4::TcpConnection(net::ipv4::SockAddr_In("127.0.0.1", 2349));
        std::vector<char> buffer1 = c2.Recv(0);
        std::vector<char> buffer2 = {'h', 'e', 'l', 'l', 'o', 'w', 'o', 'r', 'l', 'd', '!'};
        c2.Close();
        EXPECT_EQ(buffer2, buffer1);
    } catch (SocketError e) {
        LOG(ERROR) << e.what();
        FAIL();
    } catch (NetworkError e) {
        LOG(INFO) << e.what();
        FAIL();
    } catch (SockAddrError e) {
        LOG(ERROR) << e.what();
        FAIL();
    }

    t1.join();
}

TEST(net_Connection, ipv6_tcp_connection) {
    std::thread t1([&] {
        try {
            net::ipv6::TcpListener l1 = net::ipv6::TcpListener(net::ipv6::SockAddr_In6("::1", 2350));
            net::ipv6::TcpConnection c1 = l1.Accept();
            l1.Close();
            std::vector<char> buffer1 = {'h', 'e', 'l', 'l', 'o', 'w', 'o', 'r', 'l', 'd', '!'};
            c1.Send(buffer1, 0);
            c1.Close();
        } catch (SocketError e) {
            LOG(ERROR) << e.what();
            FAIL();
        } catch (NetworkError e) {
            LOG(ERROR) << e.what();
            FAIL();
        } catch (SockAddrError e) {
            LOG(ERROR) << e.what();
            FAIL();
        }
    });

    sleep(1);

    try {
        net::ipv6::TcpConnection c2 = net::ipv6::TcpConnection(net::ipv6::SockAddr_In6("::1", 2350));
        std::vector<char> buffer1 = c2.Recv(0);
        std::vector<char> buffer2 = {'h', 'e', 'l', 'l', 'o', 'w', 'o', 'r', 'l', 'd', '!'};
        c2.Close();
        EXPECT_EQ(buffer2, buffer1);
    } catch (SocketError e) {
        LOG(ERROR) << e.what();
        FAIL();
    } catch (NetworkError e) {
        LOG(INFO) << e.what();
        FAIL();
    } catch (SockAddrError e) {
        LOG(ERROR) << e.what();
        FAIL();
    }

    t1.join();
}
