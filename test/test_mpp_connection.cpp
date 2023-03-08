//
// Created by Matthias Hofstätter on 05.03.23.
//

#include <thread>
#include <glog/logging.h>

#include "gtest/gtest.h"
#include "../src/net/Socket.h"
#include "../src/net/SockAddr.h"
#include "../src/net/Connection.h"

TEST(test_mpp_connection, simple_ipv4_tcp_connection) {
    std::thread t1([&] {
        try {
            mpp::net::ipv4::TcpConnection c1 = mpp::net::ipv4::TcpConnection(mpp::net::ipv4::SockAddr_In("127.0.0.1", 2347));
            c1.Accept();
            std::vector<char> buffer1 = {'h', 'e', 'l', 'l', 'o', 'w', 'o', 'r', 'l', 'd', '!'};
            c1.Write(buffer1);
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
        mpp::net::ipv4::TcpConnection c1 = mpp::net::ipv4::TcpConnection(mpp::net::ipv4::SockAddr_In("127.0.0.1", 2348));
        c1.Connect(mpp::net::ipv4::SockAddr_In("127.0.0.1", 2347));
        std::vector<char> buffer1 = c1.Read();
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
