//
// Created by Matthias Hofstätter on 28.02.23.
//

#include <thread>
#include <vector>
#include <cstddef>
#include <glog/logging.h>

#include "gtest/gtest.h"
#include "../src/net/TcpSocket.h"


TEST(net_Socket, net_ipv4_TcpSocket) {
    std::thread t1([&] {
        try {
            net::ipv4::TcpSocket s1 = net::ipv4::TcpSocket();
            s1.SetSockOpt(SOL_SOCKET, SO_REUSEADDR, 1);
            s1.Bind(net::ipv4::SockAddr_In("127.0.0.1", 7001));
            s1.Listen();
            net::ipv4::TcpSocket s2 = s1.Accept();
            std::vector<char> buffer1 = {'h', 'e', 'l', 'l', 'o', 'w', 'o', 'r', 'l', 'd', '!'};
            s2.Write(buffer1);
            s1.Close();
            s2.Close();
        } catch (SocketError e) {
            LOG(ERROR) << e.what();
        } catch (NetworkError e) {
            LOG(ERROR) << e.what();
        } catch (SockAddrError e) {
            LOG(ERROR) << e.what();
        }
    });

    sleep(3);

    try {
        net::ipv4::TcpSocket s3 = net::ipv4::TcpSocket();
        s3.Connect(net::ipv4::SockAddr_In("127.0.0.1", 7001));
        std::vector<char> buffer1 = s3.Read();
        std::vector<char> buffer2 = {'h', 'e', 'l', 'l', 'o', 'w', 'o', 'r', 'l', 'd', '!'};
        EXPECT_EQ(buffer2, buffer1);
    } catch (SocketError e) {
        LOG(ERROR) << e.what();
    } catch (NetworkError e) {
        LOG(INFO) << e.what();
    } catch (SockAddrError e) {
        LOG(ERROR) << e.what();
    }

    t1.join();
}

TEST(net_Socket, net_ipv6_TcpSocket) {
    std::thread t1([&] {
        try {
            net::ipv6::TcpSocket s1 = net::ipv6::TcpSocket();
            s1.SetSockOpt(SOL_SOCKET, SO_REUSEADDR, 1);
            s1.Bind(net::ipv6::SockAddr_In6("::1", 6542));
            s1.Listen();
            net::ipv6::TcpSocket s2 = s1.Accept();
            std::vector<char> buffer1 = {'h', 'e', 'l', 'l', 'o', 'w', 'o', 'r', 'l', 'd', '!'};
            s2.Write(buffer1);
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
        net::ipv6::TcpSocket s3 = net::ipv6::TcpSocket();
        s3.Connect(net::ipv6::SockAddr_In6("::1", 6542));
        LOG(INFO) << "Connected to socket s1.";
        std::vector<char> buffer1 = s3.Read();
        std::vector<char> buffer2 = {'h', 'e', 'l', 'l', 'o', 'w', 'o', 'r', 'l', 'd', '!'};
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