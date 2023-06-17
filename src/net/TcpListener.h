//
// Created by Matthias Hofstätter on 04.04.23.
//

#ifndef MULTIPATH_PROXY_TCPLISTENER_H
#define MULTIPATH_PROXY_TCPLISTENER_H

#include <string>

#include "Listener.h"

namespace net {
    namespace ipv4 {

        class TcpConnection;

        class TcpListener : public Listener<TcpSocket, SockAddr_In> {
        public:
            static TcpListener *make(SockAddr_In sockaddr);

            TcpConnection *Accept();
            TcpConnection *Accept(SockAddr_In &addr);

            std::string ToString();

        protected:
            TcpListener(SockAddr_In sockaddr);
        };
    } // ipv4


    namespace ipv6 {

        class TcpListener : public Listener<TcpSocket, SockAddr_In6> {
        public:
            static TcpListener *make(SockAddr_In6 sockaddr);

            TcpConnection *Accept();
            TcpConnection *Accept(SockAddr_In6 &addr);

            std::string ToString();

        protected:
            TcpListener(SockAddr_In6 sockaddr);
        };
    } // ipv6
} // net


#endif //MULTIPATH_PROXY_TCPLISTENER_H
