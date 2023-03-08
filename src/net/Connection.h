//
// Created by Matthias Hofstätter on 04.03.23.
//

#ifndef MULTIPATH_PROXY_CONNECTION_H
#define MULTIPATH_PROXY_CONNECTION_H

#include "Socket.h"
#include "SockAddr.h"

namespace mpp::net {

    template <class S, class SA>
    class Connection : protected S, protected SA {
    public:
        void Connect(SA sockaddr) {
            S::Connect(sockaddr);
        }

        std::vector<char> Read() {
            return S::Read();
        }

        void Write(std::vector<char> buffer) {
            S::Write(buffer);
        }

        virtual ~Connection() {
            S::Close();
        }

    protected:
        Connection(SA sockaddr) : SA(sockaddr) {
            S::SetSockOpt(SOL_SOCKET, SO_REUSEADDR, 1);
            S::Bind((SA)*this);
        };
    };

    namespace ipv4 {

        class TcpConnection : protected Connection<TcpSocket, SockAddr_In> {
        public:
            TcpConnection(SockAddr_In sockaddr) : Connection<TcpSocket, SockAddr_In>(sockaddr) {}

            void Connect(SockAddr_In sockaddr) { Connection<TcpSocket, SockAddr_In>::Connect(sockaddr); }

            void Accept() { Connection<TcpSocket, SockAddr_In>::Accept(); }

            std::vector<char> Read() { return Connection<TcpSocket, SockAddr_In>::Read(); }

            void Write(std::vector<char> buffer) { Connection<TcpSocket, SockAddr_In>::Write(buffer); }
        };
    }

    namespace ipv6 {

        class TcpConnection : protected Connection<mpp::net::ipv6::TcpSocket, SockAddr_In6> {
        public:
            TcpConnection(SockAddr_In6 sockaddr) : Connection<TcpSocket, SockAddr_In6>(sockaddr) {};

            void Connect(SockAddr_In6 sockaddr) { Connection<TcpSocket, SockAddr_In6>::Connect(sockaddr); }

            void Accept() { Connection<TcpSocket, SockAddr_In6>::Accept(); }

            std::vector<char> Read() { return Connection<TcpSocket, SockAddr_In6>::Read(); }

            void Write(std::vector<char> buffer) { Connection<TcpSocket, SockAddr_In6>::Write(buffer); }
        };
    }
}


#endif //MULTIPATH_PROXY_CONNECTION_H
