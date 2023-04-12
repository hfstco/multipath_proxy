//
// Created by Matthias Hofstätter on 04.04.23.
//

#ifndef MULTIPATH_PROXY_UDPSOCKET_H
#define MULTIPATH_PROXY_UDPSOCKET_H

#include "Socket.h"
#include "SockAddr.h"


namespace net {
    namespace ipv4 {

        class UdpSocket : protected Socket<UdpSocket, SockAddr_In> {
            friend Socket<UdpSocket, SockAddr_In>;

        public:
            UdpSocket() : Socket<UdpSocket, SockAddr_In>(AF_INET, SOCK_DGRAM, 0) {};

            void Bind(SockAddr_In sockaddr);

            void Connect(SockAddr_In addr); // TODO check

            void Send(std::vector<char> buf, int flags);

            std::vector<char> Recv(int flags);

            void SendTo(std::vector<char> buf, int flags, SockAddr_In dest_addr);

            std::vector<char> RecvFrom(int flags, SockAddr_In src_addr);

            short Poll(short revents, int nfds, int timeout = 0);

            SockAddr_In GetSockName();

            SockAddr_In GetPeerName();

            template<typename T>
            void SetSockOpt(int level, int optname, T optval) {
                Socket<UdpSocket, SockAddr_In>::SetSockOpt(level, optname, optval);
            }

            void Close();

        protected:
            UdpSocket(int fd) : Socket<UdpSocket, SockAddr_In>(fd) {};
        };

    } // ipv4

    namespace ipv6 {

        class UdpSocket : protected Socket<UdpSocket, SockAddr_In6> {
            friend Socket<UdpSocket, SockAddr_In6>;

        public:
            UdpSocket() : Socket<UdpSocket, SockAddr_In6>(AF_INET, SOCK_DGRAM, 0) {};

            void Bind(SockAddr_In6 sockaddr);

            void Connect(SockAddr_In6 addr);

            void Send(std::vector<char> buf, int flags);

            std::vector<char> Recv(int flags);

            void SendTo(std::vector<char> buf, int flags, SockAddr_In6 dest_addr);

            std::vector<char> RecvFrom(int flags, SockAddr_In6 src_addr);

            short Poll(short revents, int nfds, int timeout = 0);

            SockAddr_In6 GetSockName();

            SockAddr_In6 GetPeerName();

            template<typename T>
            void SetSockOpt(int level, int optname, T optval) {
                Socket<UdpSocket, SockAddr_In6>::SetSockOpt(level, optname, optval);
            }

            void Close() { Socket<UdpSocket, SockAddr_In6>::Close(); }

        protected:
            UdpSocket(int fd) : Socket<UdpSocket, SockAddr_In6>(fd) {};
        };

    } // ipv6
} // net


#endif //MULTIPATH_PROXY_UDPSOCKET_H
