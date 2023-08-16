//
// Created by Matthias Hofstätter on 13.06.23.
//

#include "TcpConnection.h"

namespace net {
    namespace ipv4 {

        TcpConnection::TcpConnection(SockAddr_In peeraddr) : Connection<TcpSocket, SockAddr_In>(peeraddr) {
            //DLOG(INFO) << "TcpConnection(peeraddr=" << peeraddr.ToString() << ") * " << ToString();
        }

        TcpConnection::TcpConnection(SockAddr_In peeraddr, SockAddr_In sockaddr) : Connection<TcpSocket, SockAddr_In>(peeraddr, sockaddr) {
            //DLOG(INFO) << "TcpConnection(peeraddr=" << peeraddr.ToString() << ", sockaddr=" + sockaddr.ToString() + ") * " << ToString();
        }

        TcpConnection::TcpConnection(int fd) : Connection<TcpSocket, SockAddr_In>(fd) {
            //DLOG(INFO) << "TcpConnection(fd=" + std::to_string(fd) + ") * " << ToString();
        }

        TcpConnection *TcpConnection::make(SockAddr_In peeraddr) {
            return new TcpConnection(peeraddr);
        }

        TcpConnection *TcpConnection::make(SockAddr_In peeraddr, SockAddr_In sockaddr) {
            return new TcpConnection(peeraddr, sockaddr);
        }

        std::string TcpConnection::ToString() {
            return "TcpConnection[fd=" + std::to_string(Connection<TcpSocket, SockAddr_In>::fd()) + "]";
        }
    } // ipv4

    namespace ipv6 {

        TcpConnection::TcpConnection(SockAddr_In6 peeraddr) : Connection<TcpSocket, SockAddr_In6>(peeraddr) {
            //DLOG(INFO) << "TcpConnection(peeraddr=" << peeraddr.ToString() << ") * " << ToString();
        }

        TcpConnection::TcpConnection(SockAddr_In6 peeraddr, SockAddr_In6 sockaddr) : Connection<TcpSocket, SockAddr_In6>(peeraddr, sockaddr) {
            //DLOG(INFO) << "TcpConnection(peeraddr=" << peeraddr.ToString() << ", sockaddr=" + sockaddr.ToString() + ") * " << ToString();
        }

        TcpConnection::TcpConnection(int fd) : Connection<TcpSocket, SockAddr_In6>(fd) {
            //DLOG(INFO) << "TcpConnection(fd=" + std::to_string(fd) + ") * " << ToString();
        }

        TcpConnection *TcpConnection::make(SockAddr_In6 peeraddr) {
            return new TcpConnection(peeraddr);
        }

        TcpConnection *TcpConnection::make(SockAddr_In6 peeraddr, SockAddr_In6 sockaddr) {
            return new TcpConnection(peeraddr, sockaddr);
        }

        std::string TcpConnection::ToString() {
            return "TcpConnection[fd=" + std::to_string(Connection<TcpSocket, SockAddr_In6>::fd()) + "]";
        }
    } // ipv6
} // net