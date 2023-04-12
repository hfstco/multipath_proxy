//
// Created by Matthias Hofstätter on 04.04.23.
//

#ifndef MULTIPATH_PROXY_TCPLISTENER_H
#define MULTIPATH_PROXY_TCPLISTENER_H

#include "Listener.h"
#include "TcpConnection.h"


namespace net {
    namespace ipv4 {

        class TcpListener : public Listener<TcpSocket, SockAddr_In> {

        public:
            TcpListener(SockAddr_In sockaddr) : Listener<TcpSocket, SockAddr_In>(sockaddr) {}

            TcpConnection Accept(); // to keep TcpConnection(SA) constructor protected
        };

    } // ipv4


    namespace ipv6 {

        class TcpListener : public Listener<TcpSocket, SockAddr_In6> {

        public:
            TcpListener(SockAddr_In6 sockaddr) : Listener<TcpSocket, SockAddr_In6>(sockaddr) {}

            TcpConnection Accept(); // to keep TcpConnection(SA) constructor protected
        };

    } // ipv6
} // net


#endif //MULTIPATH_PROXY_TCPLISTENER_H
