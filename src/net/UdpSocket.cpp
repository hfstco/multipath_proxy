//
// Created by Matthias Hofstätter on 04.04.23.
//

#include "UdpSocket.h"


namespace net {
    namespace ipv4 {

        void UdpSocket::Bind(SockAddr_In sockaddr) {
            Socket<UdpSocket, SockAddr_In>::Bind(sockaddr);
        }

        void UdpSocket::Connect(SockAddr_In addr) {
            Socket<UdpSocket, SockAddr_In>::Connect(addr);
        }

        void UdpSocket::Send(std::vector<char> buf, int flags) {
            Socket<UdpSocket, SockAddr_In>::Send(buf, flags);
        }

        std::vector<char> UdpSocket::Recv(int flags) {
            return Socket<UdpSocket, SockAddr_In>::Recv(flags);
        }

        void UdpSocket::SendTo(std::vector<char> buf, int flags, SockAddr_In dest_addr) {
            Socket<UdpSocket, SockAddr_In>::SendTo(buf, flags, dest_addr);
        }

        std::vector<char> UdpSocket::RecvFrom(int flags, SockAddr_In src_addr) {
            return Socket<UdpSocket, SockAddr_In>::RecvFrom(flags, src_addr);
        }

        short UdpSocket::Poll(short revents, int nfds, int timeout) {
            return Socket<UdpSocket, SockAddr_In>::Poll(revents, nfds, timeout);
        }

        SockAddr_In UdpSocket::GetSockName() {
            return Socket<UdpSocket, SockAddr_In>::GetSockName();
        }

        SockAddr_In UdpSocket::GetPeerName() {
            return Socket<UdpSocket, SockAddr_In>::GetPeerName();
        }

        void UdpSocket::Close() {
            Socket<UdpSocket, SockAddr_In>::Close();
        }

    } // ipv4

    namespace ipv6 {

        void UdpSocket::Bind(SockAddr_In6 sockaddr) {
            Socket<UdpSocket, SockAddr_In6>::Bind(sockaddr);
        }

        void UdpSocket::Connect(SockAddr_In6 addr) {
            Socket<UdpSocket, SockAddr_In6>::Connect(addr);
        }

        void UdpSocket::Send(std::vector<char> buf, int flags) {
            Socket<UdpSocket, SockAddr_In6>::Send(buf, flags);
        }

        std::vector<char> UdpSocket::Recv(int flags) {
            return Socket<UdpSocket, SockAddr_In6>::Recv(flags);
        }

        void UdpSocket::SendTo(std::vector<char> buf, int flags, SockAddr_In6 dest_addr) {
            Socket<UdpSocket, SockAddr_In6>::SendTo(buf, flags, dest_addr);
        }

        std::vector<char> UdpSocket::RecvFrom(int flags, SockAddr_In6 src_addr) {
            return Socket<UdpSocket, SockAddr_In6>::RecvFrom(flags, src_addr);
        }

        short UdpSocket::Poll(short revents, int nfds, int timeout) {
            return Socket<UdpSocket, SockAddr_In6>::Poll(revents, nfds, timeout);
        }

        SockAddr_In6 UdpSocket::GetSockName() {
            return Socket<UdpSocket, SockAddr_In6>::GetSockName();
        }

        SockAddr_In6 UdpSocket::GetPeerName() {
            return Socket<UdpSocket, SockAddr_In6>::GetPeerName();
        }

    } // ipv6
} // net

