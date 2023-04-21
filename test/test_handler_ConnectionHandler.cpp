//
// Created by Matthias Hofstätter on 12.04.23.
//

#include "gtest/gtest.h"
#include "../src/net/base/TcpListener.h"
#include <thread>

TEST(handler_ConnectionHandler, simple_ConnectionHandler) {
    /*std::thread t1([&] {
        try {
            net::ipv4::TcpListener l1 = net::ipv4::TcpListener(net::ipv4::SockAddr_In("127.0.0.1", 2351));
            net::ipv4::TcpConnection c1 = l1.Accept();
            handler::ConnectionHandler ch1 = handler::ConnectionHandler(c1);
            handler::HeartBeatHandler hbh1 = handler::HeartBeatHandler(c1);

            sleep(5);
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
        net::ipv4::TcpConnection c2 = net::ipv4::TcpConnection(net::ipv4::SockAddr_In("127.0.0.1", 2351));
        handler::ConnectionHandler ch2 = handler::ConnectionHandler(c2);
        handler::HeartBeatHandler hbh1 = handler::HeartBeatHandler(c2);

        sleep(5);
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

    t1.join();*/
}
