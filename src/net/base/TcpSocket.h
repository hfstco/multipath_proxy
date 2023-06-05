//
// Created by Matthias Hofstätter on 04.04.23.
//

#ifndef MULTIPATH_PROXY_TCPSOCKET_H
#define MULTIPATH_PROXY_TCPSOCKET_H

#include "Socket.h"
#include "SockAddr.h"


namespace net {
    namespace ipv4 {


        class TcpSocket : public Socket<TcpSocket, SockAddr_In> {
            friend Socket<TcpSocket, SockAddr_In>; // S Accept()

        public:
            static TcpSocket *make();

            void Bind(SockAddr_In addr);
            void Listen(int backlog = SOMAXCONN);

            TcpSocket *Accept();
            TcpSocket *Accept(SockAddr_In &addr);
            void Connect(SockAddr_In addr);

            ssize_t Send(unsigned char *buf, size_t size, int flags);
            ssize_t Recv(unsigned char *buf, size_t size, int flags);

            short Poll(short events, int timeout = 0);

            SockAddr_In GetSockName();
            SockAddr_In GetPeerName();

            template<typename T>
            void SetSockOpt(int level, int optname, T optval) {
                Socket<TcpSocket, SockAddr_In>::SetSockOpt<T>(level, optname, optval);
            }

            template<typename T>
            T GetSockOpt(int level, int optname) {
                return Socket<TcpSocket, SockAddr_In>::GetSockOpt<T>(level, optname);
            }

            void Close();

        protected:
            TcpSocket();
            TcpSocket(int sock_fd);
        };

    } // ipv4

    namespace ipv6 {

        class TcpSocket : protected Socket<TcpSocket, SockAddr_In6> {
            friend Socket<TcpSocket, SockAddr_In6>; // S Accept()

        public:
            static TcpSocket *make();

            void Bind(SockAddr_In6 addr);
            void Listen(int backlog = SOMAXCONN);

            TcpSocket *Accept();
            TcpSocket *Accept(SockAddr_In6 &addr);
            void Connect(SockAddr_In6 addr);

            ssize_t Send(unsigned char *buf, size_t size, int flags);
            ssize_t Recv(unsigned char *buf, size_t size, int flags);

            short Poll(short revents, int timeout = 0);

            SockAddr_In6 GetSockName();
            SockAddr_In6 GetPeerName();

            template<typename T>
            void SetSockOpt(int level, int optname, T optval) {
                Socket<TcpSocket, SockAddr_In6>::SetSockOpt(level, optname, optval);
            }

            template<typename T>
            void GetSockOpt(int level, int optname, T optval) {
                Socket<TcpSocket, SockAddr_In6>::GetSockOpt(level, optname, optval);
            }

            void Close();

        protected:
            TcpSocket();
            TcpSocket(int sock_fd);
        };

    } // ipv6
}


#endif //MULTIPATH_PROXY_TCPSOCKET_H
