//
// Created by Matthias Hofstätter on 28.02.23.
//

#include <thread>
#include <vector>
#include <cstddef>
#include <glog/logging.h>

#include "gtest/gtest.h"
#include "../src/net/TcpSocket.h"


TEST(net_Socket, ipv4_TcpSocket_connect_send_recv) {
    try {
        net::ipv4::TcpSocket *pTcpSocket1 = net::ipv4::TcpSocket::make();
        pTcpSocket1->SetSockOpt(SOL_SOCKET, SO_REUSEADDR, 1);
        pTcpSocket1->Bind(net::ipv4::SockAddr_In("127.0.0.1", 7001));
        pTcpSocket1->Listen();

        net::ipv4::TcpSocket *pTcpSocket2 = net::ipv4::TcpSocket::make();

        pTcpSocket2->Connect(net::ipv4::SockAddr_In("127.0.0.1", 7001));
        net::ipv4::TcpSocket *pTcpSocket3 = pTcpSocket1->Accept();

        std::vector<unsigned char> vector1 = {'h', 'e', 'l', 'l', 'o', 'w', 'o', 'r', 'l', 'd', '!'};
        pTcpSocket3->Send(vector1.data(), vector1.size(), 0);

        std::vector<unsigned char> vector2(1024);
        ssize_t size = pTcpSocket2->Recv(vector2.data(), vector2.size(), 0);
        vector2.resize(size);

        EXPECT_EQ(vector2, vector1);

        delete pTcpSocket1;
        delete pTcpSocket2;
        delete pTcpSocket3;
    } catch (Exception e) {
        LOG(ERROR) << e.what();
        FAIL();
    }
}

/*TEST(net_Socket, ipv6_TcpSocket_connect_send_recv) {
    try {
        // create listener
        net::ipv6::TcpSocket *pTcpSocket1 = net::ipv6::TcpSocket::make();
        pTcpSocket1->SetSockOpt(SOL_SOCKET, SO_REUSEADDR, 1);
        pTcpSocket1->Bind(net::ipv6::SockAddr_In6("::1", 7002));
        pTcpSocket1->Listen();

        // create remote socket
        net::ipv6::TcpSocket *pTcpSocket2 = net::ipv6::TcpSocket::make();
        pTcpSocket2->Connect(net::ipv6::SockAddr_In6("::1", 7002));

        // accept connection on local side
        net::ipv6::TcpSocket *pTcpSocket3 = pTcpSocket1->Accept();

        // send from local -> remote
        std::vector<unsigned char> vector1 = {'h', 'e', 'l', 'l', 'o', 'w', 'o', 'r', 'l', 'SendToSat', '!'};
        pTcpSocket3->Send(vector1.data(), vector1.size(), 0);

        // recv on remote side
        std::vector<unsigned char> vector2(1024);
        ssize_t size1 = pTcpSocket2->Recv(vector2.data(), vector2.size(), 0);
        vector2.resize(size1);

        // send back to local side
        pTcpSocket2->Send(vector2.data(), vector2.size(), 0);

        // recv on remote side
        std::vector<unsigned char> vector3(1024);
        ssize_t size2 = pTcpSocket3->Recv(vector3.data(), vector3.size(), 0);
        vector3.resize(size2);

        EXPECT_EQ(vector3, vector1);

        delete pTcpSocket1;
        delete pTcpSocket2;
        delete pTcpSocket3;
    } catch (Exception e) {
        LOG(ERROR) << e.what();
        FAIL();
    }
}*/

TEST(net_Socket, ipv4_TcpSocket_recv_closed_socket) {
    try {
        // create listener
        net::ipv4::TcpSocket *pTcpSocket1 = net::ipv4::TcpSocket::make();
        pTcpSocket1->SetSockOpt(SOL_SOCKET, SO_REUSEADDR, 1);
        pTcpSocket1->Bind(net::ipv4::SockAddr_In("127.0.0.1", 7001));
        pTcpSocket1->Listen();

        // create remote connection
        net::ipv4::TcpSocket *pTcpSocket2 = net::ipv4::TcpSocket::make();
        pTcpSocket2->Connect(net::ipv4::SockAddr_In("127.0.0.1", 7001));

        // accept remote connection on local side
        net::ipv4::TcpSocket *pTcpSocket3 = pTcpSocket1->Accept();
        // close listener
        delete pTcpSocket1;

        // close remote connection
        delete pTcpSocket2;

        // try to read local socket with closed remote socket
        std::vector<unsigned char> vector1(1024);
        int size = pTcpSocket3->Recv(vector1.data(), vector1.size(), 0);
        EXPECT_EQ(0, size);

        // close local socket
        delete pTcpSocket3;
    } catch (Exception e) {
        LOG(ERROR) << e.what();
        FAIL();
    }
}

TEST(net_Socket, ipv4_TcpSocket_send_closed_socket) {
    try {
        // create listener
        net::ipv4::TcpSocket *pTcpSocket1 = net::ipv4::TcpSocket::make();
        pTcpSocket1->SetSockOpt(SOL_SOCKET, SO_REUSEADDR, 1);
        pTcpSocket1->Bind(net::ipv4::SockAddr_In("127.0.0.1", 7001));
        pTcpSocket1->Listen();

        // create remote connection
        net::ipv4::TcpSocket *pTcpSocket2 = net::ipv4::TcpSocket::make();
        pTcpSocket2->Connect(net::ipv4::SockAddr_In("127.0.0.1", 7001));

        // accept remote connection on local side
        net::ipv4::TcpSocket *pTcpSocket3 = pTcpSocket1->Accept();
        // close listener
        delete pTcpSocket1;

        // close remote connection
        delete pTcpSocket2;

        // try to read local socket with closed remote socket
        std::vector<unsigned char> vector1 = {'a'};
        pTcpSocket3->Send(vector1.data(), vector1.size(), 0);

        // close local socket
        delete pTcpSocket3;
    } catch (Exception e) {
        LOG(ERROR) << e.what();
        FAIL();
    }
}