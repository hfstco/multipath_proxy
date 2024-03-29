//
// Created by Matthias Hofstätter on 04.04.23.
//

#ifndef MULTIPATH_PROXY_TCPCONNECTION_H
#define MULTIPATH_PROXY_TCPCONNECTION_H

#include "Connection.h"

#include "../metrics/Metrics.h"

namespace net {
    namespace ipv4 {

        class TcpListener;

        class TcpConnection : public Connection<TcpSocket, SockAddr_In> {
            //friend TcpListener; // allow TcpListener to call protected TcpConnection(TcpSocket socket)
            friend Socket<TcpSocket, SockAddr_In>;

        public:
            static TcpConnection *make(SockAddr_In peeraddr);
            static TcpConnection *make(SockAddr_In peeraddr, SockAddr_In sockaddr);

            std::string ToString();

        protected:
            TcpConnection(SockAddr_In peeraddr);
            TcpConnection(SockAddr_In peeraddr, SockAddr_In sockaddr);

        private:
            TcpConnection(int fd);
        };

    } // ipv4

    namespace ipv6 {

        class TcpListener;

        class TcpConnection : public Connection<TcpSocket, SockAddr_In6> {
            //friend TcpListener; // allow TcpListener to call protected TcpConnection(TcpSocket socket)
            friend Socket<TcpSocket, SockAddr_In6>;

        public:
            static TcpConnection *make(SockAddr_In6 sockaddr);
            static TcpConnection *make(SockAddr_In6 peeraddr, SockAddr_In6 sockaddr);

            std::string ToString();

        protected:
            TcpConnection(SockAddr_In6 sockaddr);
            TcpConnection(SockAddr_In6 peeraddr, SockAddr_In6 sockaddr);

        private:
            TcpConnection(int fd);
        };

    } // ipv6
} // net

#endif //MULTIPATH_PROXY_TCPCONNECTION_H
