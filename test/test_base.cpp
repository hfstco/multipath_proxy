//
// Created by Matthias Hofstätter on 04.04.23.
//

#include <thread>
#include "gtest/gtest.h"
#include "../src/base/Remote.h"
#include "../src/base/Local.h"

TEST(base, simple_connection) {
    /*std::thread t1([&] {
        try {
            net::ipv4::TcpListener ter1 = net::ipv4::TcpListener(net::ipv4::SockAddr_In("127.0.0.1", 5678));
            net::ipv4::TcpListener sat1 = net::ipv4::TcpListener(net::ipv4::SockAddr_In("127.0.0.1", 5679));
            base::Remote r1 = base::Remote(ter1, sat1);
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

    sleep(1);

    try {
        net::ipv4::TcpListener proxy1 = net::ipv4::TcpListener(net::ipv4::SockAddr_In("127.0.0.1", 5000));
        net::ipv4::TcpConnection ter1 = net::ipv4::TcpConnection(net::ipv4::SockAddr_In("127.0.0.1", 5678));
        net::ipv4::TcpConnection sat1 = net::ipv4::TcpConnection(net::ipv4::SockAddr_In("127.0.0.1", 5679));
        base::Local l1 = base::Local(proxy1, ter1, sat1);
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

    t1.join();*/
}