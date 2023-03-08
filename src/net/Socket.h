//
// Created by Matthias Hofstätter on 27.02.23.
//

#ifndef MULTIPATH_PROXY_SOCKET_H
#define MULTIPATH_PROXY_SOCKET_H

#include <vector>
#include <unistd.h>
#include <poll.h>
#include <sys/socket.h>

#include "glog/logging.h"

#include "SockAddr.h"

namespace mpp::net {

    template<class S, class SA>
    class Socket {
    public:
        void Bind(SA sockaddr) {
            if (bind(this->sock_fd_, (struct sockaddr *) &sockaddr, sizeof(SA)) < 0) {
                throw SocketError("Bind failed.");
            }

            LOG(INFO) << "Bind() S[fd=" << this->sock_fd_ << "] to (" << sockaddr.ip() << ":" << sockaddr.port() << ")";
        }

        S Accept() { // TODO merge with Accept(SA &sockaddr)
            int temp_fd = -1;

            LOG(INFO) << "Accept() S[fd=" << this->sock_fd_ << "]";

            if ((temp_fd = accept(this->sock_fd_, NULL, NULL)) < 0) {
                throw SocketError("Accept failed.");
            }

            return S(temp_fd);
        };

        S Accept(SA &sockaddr) {
            int temp_fd = -1;
            socklen_t socklen = sizeof(sockaddr);

            LOG(INFO) << "Accept() S[fd=" << this->sock_fd_ << "]";

            if ((temp_fd = accept(this->sock_fd_, (struct sockaddr *) &sockaddr, &socklen)) < 0) {
                throw SocketError("Accept failed.");
            }

            return S(temp_fd);
        };

        void Listen(int backlog = SOMAXCONN) {
            if (listen(this->sock_fd_, backlog) < 0) {
                throw SocketError("Listen failed.");
            }

            LOG(INFO) << "Listen() S[fd=" << this->sock_fd_ << "]";
        }

        void Connect(SA sockaddr) {
            if (connect(this->sock_fd_, (struct sockaddr *) &sockaddr, sizeof(sockaddr)) < 0) {
                throw SocketError("Connect failed.");
            }

            LOG(INFO) << "Connect() S[fd=" << this->sock_fd_ << "] to (" << sockaddr.ip() << ":" << sockaddr.port()
                      << ")";
        }

        std::vector<char> Read() {
            std::vector<char> buffer(1000); // TODO buffer size
            int bytes_read = 0;

            if ((bytes_read = read(this->sock_fd_, &buffer[0], 1000)) < 0) { // TODO buffer size
                throw SocketError("Read error.");
            }

            buffer.resize(bytes_read);

            LOG(INFO) << "Read() S[fd=" << this->sock_fd_ << "]";

            return buffer;
        }

        void Write(std::vector<char> buffer) {
            if (write(this->sock_fd_, &buffer[0], buffer.size()) < 0) {
                throw SocketError("Write error.");
            }

            LOG(INFO) << "Write() S[fd=" << this->sock_fd_ << "]";
        }

        short Poll(short revents, int nfds, int timeout = 0) {
            pollfd poll_fd[2];
            poll_fd[0].fd = this->sock_fd_;
            poll_fd[0].revents = revents;

            if (::poll(poll_fd, nfds, timeout) < 0) {
                throw SocketError("Poll failed.");
            }

            return 0;
        }

        template<typename T>
        T GetSockName() {
            T sockaddr;
            socklen_t sockaddr_len = sizeof(sockaddr);

            if (getsockname(this->sock_fd_, (struct ::sockaddr *) &sockaddr, &sockaddr_len) < 0) {
                throw SocketError("GetSockName failed.");
            }

            return sockaddr;
        }

        template<typename T>
        T GetPeerName() {
            T sockaddr;
            socklen_t sockaddr_len = sizeof(sockaddr);

            if (getpeername(this->sock_fd_, (struct ::sockaddr *) &sockaddr, &sockaddr_len) < 0) {
                throw SocketError("GetSockName failed.");
            }

            return sockaddr;
        }

        template<typename T>
        void SetSockOpt(int level, int optname, T optval) {
            if (setsockopt(this->sock_fd_, level, optname, (void *) &optval, sizeof(T)) < 0) {
                throw SocketError("SetSockOpt failed.");
            }
        }

        void Close() {
            close(this->sock_fd_);

            LOG(INFO) << "Close() S[fd=" << this->sock_fd_ << "]";
        }

    protected:
        Socket(int domain, int type, int protocol) {
            if ((this->sock_fd_ = ::socket(domain, type, protocol)) < 0) {
                throw SocketError("Connect failed.");
            }

            LOG(INFO) << "socket(domain=" << domain << ", type=" << type << ", protocol=" << protocol << ")" << " S[fd="
                      << this->sock_fd_ << "]";
        }

        Socket(int sock_fd) {
            this->sock_fd_ = sock_fd;

            LOG(INFO) << "socket(sockfd=" << sock_fd << ")" << " S[fd=" << this->sock_fd_ << "]";
        }

    private:
        int sock_fd_ = -1;
    };

    namespace ipv4 {
        class TcpSocket : public Socket<TcpSocket, SockAddr_In> {
            friend Socket<TcpSocket, SockAddr_In>;
        public:
            TcpSocket() : Socket<TcpSocket, SockAddr_In>(AF_INET, SOCK_STREAM, 0) {};

            void Bind(SockAddr_In sockaddr) { Socket<TcpSocket, SockAddr_In>::Bind(sockaddr); }

            TcpSocket Accept() { return Socket<TcpSocket, SockAddr_In>::Accept(); }
            TcpSocket Accept(SockAddr_In &sockaddr) { return Socket<TcpSocket, SockAddr_In>::Accept(sockaddr); }

            void connect(SockAddr_In sockaddr) { Socket<TcpSocket, SockAddr_In>::Connect(sockaddr); }

            SockAddr_In getsockname() { return Socket<TcpSocket, SockAddr_In>::GetSockName<SockAddr_In>(); }

            SockAddr_In getpeername() { return Socket<TcpSocket, SockAddr_In>::GetPeerName<SockAddr_In>(); }

            void Close() { Socket<TcpSocket, SockAddr_In>::Close(); }

        protected:
            TcpSocket(int sock_fd) : Socket<TcpSocket, SockAddr_In>(sock_fd) {};
        };
    }

    namespace ipv6 {
        class TcpSocket : public Socket<TcpSocket, SockAddr_In6> {
            friend Socket<TcpSocket, SockAddr_In6>;
        public:
            TcpSocket() : Socket<TcpSocket, SockAddr_In6>(AF_INET, SOCK_STREAM, 0) {};

            void Bind(SockAddr_In6 sockaddr) { Socket<TcpSocket, SockAddr_In6>::Bind(sockaddr); }

            TcpSocket Accept() { return Socket<TcpSocket, SockAddr_In6>::Accept(); }
            TcpSocket Accept(SockAddr_In6 &sockaddr) { return Socket<TcpSocket, SockAddr_In6>::Accept(sockaddr); }

            void connect(SockAddr_In6 sockaddr) { Socket<TcpSocket, SockAddr_In6>::Connect(sockaddr); }

            SockAddr_In6 getsockname() { return Socket<TcpSocket, SockAddr_In6>::GetSockName<SockAddr_In6>(); }

            SockAddr_In6 getpeername() { return Socket<TcpSocket, SockAddr_In6>::GetPeerName<SockAddr_In6>(); }

            void Close() { Socket<TcpSocket, SockAddr_In6>::Close(); }

        protected:
            TcpSocket(int sock_fd) : Socket<TcpSocket, SockAddr_In6>(sock_fd) {};
        };
    }
}


#endif //MULTIPATH_PROXY_SOCKET_H
