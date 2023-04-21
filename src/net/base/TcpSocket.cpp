//
// Created by Matthias Hofstätter on 04.04.23.
//

#include "TcpSocket.h"


namespace net {
    namespace ipv4 {

        void TcpSocket::Bind(net::ipv4::SockAddr_In addr) {
            Socket<TcpSocket, SockAddr_In>::Bind(addr);
        }

        void TcpSocket::Listen(int backlog) {
            return Socket<TcpSocket, SockAddr_In>::Listen(backlog);
        }

        TcpSocket *TcpSocket::Accept() {
            return (TcpSocket *)Socket<TcpSocket, SockAddr_In>::Accept();
        }

        TcpSocket *TcpSocket::Accept(SockAddr_In &addr) {
            return Socket<TcpSocket, SockAddr_In>::Accept(addr);
        }

        void TcpSocket::Connect(SockAddr_In addr) {
            Socket<TcpSocket, SockAddr_In>::Connect(addr);
        }

        ssize_t TcpSocket::Send(unsigned char *buf, size_t size, int flags) {
            return Socket<TcpSocket, SockAddr_In>::Send(buf, size, flags);
        }

        ssize_t TcpSocket::Recv(unsigned char *buf, size_t size, int flags) {
            return Socket<TcpSocket, SockAddr_In>::Recv(buf, size, flags);
        }

        short TcpSocket::Poll(short revents, int nfds, int timeout) {
            return Socket<TcpSocket, SockAddr_In>::Poll(revents, nfds, timeout);
        }

        SockAddr_In TcpSocket::GetSockName() {
            return Socket<TcpSocket, SockAddr_In>::GetSockName();
        }

        SockAddr_In TcpSocket::GetPeerName() {
            return Socket<TcpSocket, SockAddr_In>::GetPeerName();
        }

        void TcpSocket::Close() {
            Socket::Close();
        }

        TcpSocket::TcpSocket(int sock_fd) : Socket<TcpSocket, SockAddr_In>(sock_fd) {}

    } // ipv4


    namespace ipv6 {

        void TcpSocket::Bind(SockAddr_In6 addr) {
            Socket<TcpSocket, SockAddr_In6>::Bind(addr);
        }

        void TcpSocket::Listen(int backlog) {
            return Socket<TcpSocket, SockAddr_In6>::Listen(backlog);
        }

        TcpSocket *TcpSocket::Accept() {
            return Socket<TcpSocket, SockAddr_In6>::Accept();
        }

        TcpSocket *TcpSocket::Accept(SockAddr_In6 &addr) {
            return Socket::Accept(addr);
        }

        void TcpSocket::Connect(SockAddr_In6 addr) {
            Socket<TcpSocket, SockAddr_In6>::Connect(addr);
        }

        ssize_t TcpSocket::Send(unsigned char *buf, size_t size, int flags) {
            return Socket<TcpSocket, SockAddr_In6>::Send(buf, size, flags);
        }

        ssize_t TcpSocket::Recv(unsigned char *buf, size_t size, int flags) {
            return Socket<TcpSocket, SockAddr_In6>::Recv(buf, size, flags);
        }

        short TcpSocket::Poll(short revents, int nfds, int timeout) {
            return Socket<TcpSocket, SockAddr_In6>::Poll(revents, nfds, timeout);
        }

        SockAddr_In6 TcpSocket::GetSockName() {
            return Socket<TcpSocket, SockAddr_In6>::GetSockName();
        }

        SockAddr_In6 TcpSocket::GetPeerName() {
            return Socket<TcpSocket, SockAddr_In6>::GetPeerName();
        }

        void TcpSocket::Close() {
            Socket::Close();
        }

        TcpSocket::TcpSocket(int sock_fd) : Socket<TcpSocket, SockAddr_In6>(sock_fd) {}

    } // ipv6
} // net

