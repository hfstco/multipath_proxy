//
// Created by Matthias Hofstätter on 17.06.23.
//

#include "TcpListener.h"
#include "TcpConnection.h"

namespace net {
    namespace ipv4 {

        TcpListener::TcpListener(SockAddr_In sockaddr) : Listener<TcpSocket, SockAddr_In>(sockaddr) {
            DLOG(INFO) << "TcpListener(" << sockaddr.to_string() << ") * " + ToString();
        }

        TcpListener *TcpListener::make(SockAddr_In sockaddr) {
            return new TcpListener(sockaddr);
        }

        TcpConnection *TcpListener::Accept() { // to keep TcpConnection(SA) constructor protected
            /*TcpSocket *tcpSocket = Listener<TcpSocket, SockAddr_In>::Accept();
            TcpConnection *tcpConnection = new TcpConnection(tcpSocket);
            delete tcpSocket;
            return tcpConnection;*/
            return Listener<TcpSocket, SockAddr_In>::Accept<TcpConnection>();
        }

        TcpConnection *TcpListener::Accept(SockAddr_In &addr) { // to keep TcpConnection(SA) constructor protected
            /*TcpSocket *tcpSocket = Listener<TcpSocket, SockAddr_In>::Accept(addr);
            TcpConnection *tcpConnection = new TcpConnection(tcpSocket);
            delete tcpSocket;
            return tcpConnection;*/
            return Listener<TcpSocket, SockAddr_In>::Accept<TcpConnection>(addr);
        }

        std::string TcpListener::ToString() {
            return "TcpListener[fd=" + std::to_string(fd()) + "]";
        }
    } // ipv4

    namespace ipv6 {

        TcpListener::TcpListener(SockAddr_In6 sockaddr) : Listener<TcpSocket, SockAddr_In6>(sockaddr) {
            DLOG(INFO) << "TcpListener(" << sockaddr.to_string() << ") * " + ToString();
        }

        TcpListener *TcpListener::make(SockAddr_In6 sockaddr) {
            return new TcpListener(sockaddr);
        }

        TcpConnection *TcpListener::Accept() { // to keep TcpConnection(SA) constructor protected
            /*TcpSocket *tcpSocket = Listener<TcpSocket, SockAddr_In>::Accept();
            TcpConnection *tcpConnection = new TcpConnection(tcpSocket);
            delete tcpSocket;
            return tcpConnection;*/
            return Listener<TcpSocket, SockAddr_In6>::Accept<TcpConnection>();
        }

        TcpConnection *TcpListener::Accept(SockAddr_In6 &addr) { // to keep TcpConnection(SA) constructor protected
            /*TcpSocket *tcpSocket = Listener<TcpSocket, SockAddr_In>::Accept(addr);
            TcpConnection *tcpConnection = new TcpConnection(tcpSocket);
            delete tcpSocket;
            return tcpConnection;*/
            return Listener<TcpSocket, SockAddr_In6>::Accept<TcpConnection>(addr);
        }

        std::string TcpListener::ToString() {
            return "TcpListener[fd=" + std::to_string(fd()) + "]";
        }
    } // ipv6
} // net