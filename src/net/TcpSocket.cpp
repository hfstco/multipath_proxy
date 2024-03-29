//
// Created by Matthias Hofstätter on 04.04.23.
//

#include "TcpSocket.h"


namespace net {
    namespace ipv4 {

        TcpSocket::TcpSocket() : Socket<TcpSocket, SockAddr_In>(AF_INET, SOCK_STREAM, 0) {
            //DLOG(INFO) << "TcpSocket() * " + ToString();
        }

        TcpSocket::TcpSocket(int sock_fd) : Socket<TcpSocket, SockAddr_In>(sock_fd) {
            //DLOG(INFO) << "TcpSocket(fd=" + std::to_string(sock_fd) + ") * " + ToString();
        }

        TcpSocket *TcpSocket::make() {
            return new TcpSocket();
        };

        void TcpSocket::Bind(net::ipv4::SockAddr_In addr) {
            Socket<TcpSocket, SockAddr_In>::Bind(addr);
        }

        void TcpSocket::Listen(int backlog) {
            return Socket<TcpSocket, SockAddr_In>::Listen(backlog);
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

        short TcpSocket::Poll(short events, int timeout) {
            return Socket<TcpSocket, SockAddr_In>::Poll(events, timeout);
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

        std::string TcpSocket::ToString() {
            return "TcpSocket[fd=" + std::to_string(fd()) + "]";
        }

        TcpSocket::~TcpSocket() {
            //DLOG(INFO) << ToString() << ".~TcpSocket()";
        }

    } // ipv4


    namespace ipv6 {
        TcpSocket::TcpSocket() : Socket<TcpSocket, SockAddr_In6>(AF_INET6, SOCK_STREAM, 0) {
            //DLOG(INFO) << "TcpSocket() * " + ToString();
        }
        TcpSocket::TcpSocket(int sock_fd) : Socket<TcpSocket, SockAddr_In6>(sock_fd) {
            //DLOG(INFO) << "TcpSocket(" + std::to_string(sock_fd) + ") * " + ToString();
        }

        TcpSocket *TcpSocket::make() {
            return new TcpSocket();
        };

        void TcpSocket::Bind(SockAddr_In6 addr) {
            Socket<TcpSocket, SockAddr_In6>::Bind(addr);
        }

        void TcpSocket::Listen(int backlog) {
            return Socket<TcpSocket, SockAddr_In6>::Listen(backlog);
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

        short TcpSocket::Poll(short revents, int timeout) {
            return Socket<TcpSocket, SockAddr_In6>::Poll(revents, timeout);
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

        std::string TcpSocket::ToString() {
            return "TcpSocket[fd=" + std::to_string(fd()) + "]";
        }

        TcpSocket::~TcpSocket() {
            //DLOG(INFO) << ToString() << ".~TcpSocket()";
        }

    } // ipv6
} // net

