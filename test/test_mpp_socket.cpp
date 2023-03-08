//
// Created by Matthias Hofstätter on 28.02.23.
//

#include <thread>
#include <vector>
#include <cstddef>
#include <glog/logging.h>

#include "gtest/gtest.h"
#include "../src/net/SockAddr.h"
#include "../src/net/Socket.h"


TEST(test_mpp_socket, simple_ipv4_tcp_socket) {
    std::thread t1([&] {
        try {
            mpp::net::ipv4::TcpSocket s1 = mpp::net::ipv4::TcpSocket();
            s1.SetSockOpt(SOL_SOCKET, SO_REUSEADDR, 1);
            s1.Bind(mpp::net::ipv4::SockAddr_In("127.0.0.1", 7001));
            s1.Listen();
            mpp::net::ipv4::TcpSocket s2 = s1.Accept();
            std::vector<char> buffer1 = {'h', 'e', 'l', 'l', 'o', 'w', 'o', 'r', 'l', 'd', '!'};
            s2.Write(buffer1);
            s1.Close();
            s2.Close();
        } catch (mpp::SocketError e) {
            LOG(ERROR) << e.what();
        } catch (mpp::NetworkError e) {
            LOG(ERROR) << e.what();
        } catch (mpp::SockAddrError e) {
            LOG(ERROR) << e.what();
        }
    });

    sleep(3);

    try {
        mpp::net::ipv4::TcpSocket s2 = mpp::net::ipv4::TcpSocket();
        s2.Connect(mpp::net::ipv4::SockAddr_In("127.0.0.1", 7001));
        std::vector<char> buffer1 = s2.Read();
        std::vector<char> buffer2 = {'h', 'e', 'l', 'l', 'o', 'w', 'o', 'r', 'l', 'd', '!'};
        EXPECT_EQ(buffer2, buffer1);
    } catch (mpp::SocketError e) {
        LOG(ERROR) << e.what();
    } catch (mpp::NetworkError e) {
        LOG(INFO) << e.what();
    } catch (mpp::SockAddrError e) {
        LOG(ERROR) << e.what();
    }

    t1.join();
}

TEST(test_mpp_socket, simple_ipv6_tcp_socket) {
    std::thread t1([&] {
        try {
            mpp::net::ipv6::TcpSocket s1 = mpp::net::ipv6::TcpSocket();
            s1.SetSockOpt(SOL_SOCKET, SO_REUSEADDR, 1);
            s1.Bind(mpp::net::ipv6::SockAddr_In6("::1", 6542));
            LOG(INFO) << "Bound socket s1.";
            s1.Listen();
            LOG(INFO) << "Socket s1 listening...";
            s1.Accept();
            LOG(INFO) << "Accepted connection on socket s1.";
            std::vector<char> buffer1 = {'h', 'e', 'l', 'l', 'o', 'w', 'o', 'r', 'l', 'd', '!'};
            s1.Write(buffer1);
        } catch (mpp::SocketError e) {
            LOG(ERROR) << e.what();
        } catch (mpp::NetworkError e) {
            LOG(ERROR) << e.what();
        } catch (mpp::SockAddrError e) {
            LOG(ERROR) << e.what();
        }
    });

    sleep(1);

    try {
        mpp::net::ipv6::TcpSocket s2 = mpp::net::ipv6::TcpSocket();
        s2.Connect(mpp::net::ipv6::SockAddr_In6("::1", 6542));
        LOG(INFO) << "Connected to socket s1.";
        std::vector<char> buffer1 = s2.Read();
        std::vector<char> buffer2 = {'h', 'e', 'l', 'l', 'o', 'w', 'o', 'r', 'l', 'd', '!'};
        EXPECT_EQ(buffer2, buffer1);
    } catch (mpp::SocketError e) {
        LOG(ERROR) << e.what();
    } catch (mpp::NetworkError e) {
        LOG(INFO) << e.what();
    } catch (mpp::SockAddrError e) {
        LOG(ERROR) << e.what();
    }

    t1.join();
}