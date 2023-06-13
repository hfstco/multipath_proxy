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
            friend TcpListener; // allow TcpListener to call protected TcpConnection(TcpSocket socket)

        public:
            static TcpConnection *make(SockAddr_In peeraddr) {
                return new TcpConnection(peeraddr);
            }

            static TcpConnection *make(SockAddr_In peeraddr, SockAddr_In sockaddr) {
                return new TcpConnection(peeraddr, sockaddr);
            }

            std::string ToString() {
                return "TcpConnection[fd=" + std::to_string(Connection<TcpSocket, SockAddr_In>::fd()) + "]";
            }

        protected:
            TcpConnection(SockAddr_In peeraddr) : Connection<TcpSocket, SockAddr_In>(peeraddr) {
                DLOG(INFO) << "TcpConnection(peeraddr=" << peeraddr.ToString() << ") * " << ToString();
            }
            TcpConnection(SockAddr_In peeraddr, SockAddr_In sockaddr) : Connection<TcpSocket, SockAddr_In>(peeraddr, sockaddr) {
                DLOG(INFO) << "TcpConnection(peeraddr=" << peeraddr.ToString() << ", sockaddr=" + sockaddr.ToString() + ") * " << ToString();
            }

            TcpConnection(TcpSocket *socket) : Connection<TcpSocket, SockAddr_In>(socket) {
                DLOG(INFO) << "TcpConnection(socket=" << socket->ToString() << ") * " << ToString();
            }
        };

    } // ipv4

    namespace ipv6 {

        class TcpListener;

        class TcpConnection : public Connection<TcpSocket, SockAddr_In6> {
            friend TcpListener; // allow TcpListener to call protected TcpConnection(TcpSocket socket)

        public:
            static TcpConnection *make(SockAddr_In6 sockaddr) {
                return new TcpConnection(sockaddr);
            }

            static TcpConnection *make(SockAddr_In6 peeraddr, SockAddr_In6 sockaddr) {
                return new TcpConnection(peeraddr, sockaddr);
            }

        protected:
            TcpConnection(SockAddr_In6 sockaddr) : Connection<TcpSocket, SockAddr_In6>(sockaddr) {};
            TcpConnection(SockAddr_In6 peeraddr, SockAddr_In6 sockaddr) : Connection<TcpSocket, SockAddr_In6>(peeraddr, sockaddr) {}

            TcpConnection(TcpSocket *socket) : Connection<TcpSocket, SockAddr_In6>(socket) {}
        };

    } // ipv6
} // net

#endif //MULTIPATH_PROXY_TCPCONNECTION_H
