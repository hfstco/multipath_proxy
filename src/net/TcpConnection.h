//
// Created by Matthias Hofstätter on 04.04.23.
//

#ifndef MULTIPATH_PROXY_TCPCONNECTION_H
#define MULTIPATH_PROXY_TCPCONNECTION_H

#include "Connection.h"


namespace net {
    namespace ipv4 {

        class TcpListener;

        class TcpConnection : public Connection<TcpSocket, SockAddr_In> {
            friend TcpListener;

        public:
            TcpConnection(SockAddr_In peeraddr) : Connection<TcpSocket, SockAddr_In>(peeraddr) {}
            TcpConnection(SockAddr_In peeraddr, SockAddr_In sockaddr) : Connection<TcpSocket, SockAddr_In>(peeraddr, sockaddr) {}

        protected:
            TcpConnection(TcpSocket socket) : Connection<TcpSocket, SockAddr_In>(socket) {}
        };

        class UdpConnection : public Connection<UdpSocket, SockAddr_In> {

        public:
            UdpConnection(SockAddr_In peeraddr) : Connection<UdpSocket, SockAddr_In>(peeraddr) {}
            UdpConnection(SockAddr_In peeraddr, SockAddr_In sockaddr) : Connection<UdpSocket, SockAddr_In>(peeraddr, sockaddr) {}
        };

    } // ipv4

    namespace ipv6 {

        class TcpListener;

        class TcpConnection : public Connection<TcpSocket, SockAddr_In6> {
            friend TcpListener;

        public:
            TcpConnection(SockAddr_In6 sockaddr) : Connection<TcpSocket, SockAddr_In6>(sockaddr) {};
            TcpConnection(SockAddr_In6 peeraddr, SockAddr_In6 sockaddr) : Connection<TcpSocket, SockAddr_In6>(peeraddr, sockaddr) {}

        protected:
            TcpConnection(TcpSocket socket) : Connection<TcpSocket, SockAddr_In6>(socket) {}
        };

        class UdpConnection : public Connection<UdpSocket, SockAddr_In6> {

        public:
            UdpConnection(SockAddr_In6 peeraddr) : Connection<UdpSocket, SockAddr_In6>(peeraddr) {}
            UdpConnection(SockAddr_In6 peeraddr, SockAddr_In6 sockaddr) : Connection<UdpSocket, SockAddr_In6>(peeraddr, sockaddr) {}
        };

    } // ipv6
} // net

#endif //MULTIPATH_PROXY_TCPCONNECTION_H
