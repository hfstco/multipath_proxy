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
            static TcpListener *make(SockAddr_In sockaddr) {
                return new TcpListener(sockaddr);
            }

            TcpConnection *Accept() { // to keep TcpConnection(SA) constructor protected
                return (TcpConnection *)Listener<TcpSocket, SockAddr_In>::Accept();
            }

            TcpConnection *Accept(SockAddr_In &addr) { // to keep TcpConnection(SA) constructor protected
                return (TcpConnection *)Listener<TcpSocket, SockAddr_In>::Accept(addr);
            }

        protected:
            TcpListener(SockAddr_In sockaddr) : Listener<TcpSocket, SockAddr_In>(sockaddr) {}
        };

    } // ipv4


    namespace ipv6 {

        class TcpListener : public Listener<TcpSocket, SockAddr_In6> {

        public:
            static TcpListener *make(SockAddr_In6 sockaddr) {
                return new TcpListener(sockaddr);
            }

            TcpConnection *Accept() { // to keep TcpConnection(SA) constructor protected
                return (TcpConnection *)Listener<TcpSocket, SockAddr_In6>::Accept();
            }

            TcpConnection *Accept(SockAddr_In6 &addr) { // to keep TcpConnection(SA) constructor protected
                return (TcpConnection *)Listener<TcpSocket, SockAddr_In6>::Accept(addr);
            }

        protected:
            TcpListener(SockAddr_In6 sockaddr) : Listener<TcpSocket, SockAddr_In6>(sockaddr) {}
        };

    } // ipv6
} // net


#endif //MULTIPATH_PROXY_TCPLISTENER_H
