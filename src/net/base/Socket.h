//
// Created by Matthias Hofstätter on 27.02.23.
//

#ifndef MULTIPATH_PROXY_SOCKET_H
#define MULTIPATH_PROXY_SOCKET_H

#include <vector>
#include <unistd.h>
#include <poll.h>
#include <sys/socket.h>
#include <glog/logging.h>
#include "../../exception/Exception.h"
#include "SockAddr.h"

namespace net {

    namespace ipv4 {
        class TcpSocket;
        class UdpSocket;
    }
    namespace ipv6 {
        class TcpSocket;
        class UdpSocket;
    }

    template<class S>
    concept IsSocket = std::same_as<ipv4::TcpSocket, S> || std::same_as<ipv6::TcpSocket, S> || std::same_as<ipv4::UdpSocket, S> || std::same_as<ipv6::UdpSocket, S> || std::same_as<int, S>;

    template<class SA>
    concept IsSockAddr = std::same_as<SockAddr, SA> || std::same_as<ipv4::SockAddr_In, SA> || std::same_as<ipv6::SockAddr_In6, SA> || std::same_as<sockaddr*, SA>; // TODO

    template<IsSocket S = int, IsSockAddr SA = sockaddr*> // test default socket
    class Socket {

    public:
        static S *make(int domain, int type, int protocol) {
            return new Socket(domain, type, protocol);
        }

        int fd() const {
            return fd_;
        }

        void Bind(SA addr) {
            if (bind(this->fd_, (struct sockaddr *) &addr, sizeof(SA)) < 0) {
                throw SocketErrorException("Bind failed." + std::string(" errno=") + std::to_string(errno) + " (" + std::string(strerror(errno)) + ")");
            }

            LOG(INFO) << "Bind() S[fd=" << this->fd_ << "] to (" << addr.ip() << ":" << addr.port() << ")";
        }

        S *Accept() { // TODO merge with Accept(SA &sockaddr)
            int temp_fd = -1;

            if ((temp_fd = accept(this->fd_, NULL, NULL)) < 0) {
                throw SocketErrorException("Accept failed." + std::string(" errno=") + std::to_string(errno) + " (" + std::string(strerror(errno)) + ")");
            }

            LOG(INFO) << "Accept() -> S[fd=" << temp_fd << "]; S[fd=" << this->fd_ << "]";

            S *s = new S(temp_fd);
            return s;
        };

        S *Accept(SA &addr) {
            int temp_fd = -1;
            socklen_t addrlen = sizeof(addr);

            if ((temp_fd = accept(this->fd_, (struct sockaddr *) &addr, &addrlen)) < 0) {
                throw SocketErrorException("Accept failed." + std::string(" errno=") + std::to_string(errno) + " (" + std::string(strerror(errno)) + ")");
            }

            LOG(INFO) << "Accept() -> S[fd=" << temp_fd << "]; S[fd=" << this->fd_ << "]";

            return new S(temp_fd);
        };

        void Listen(int backlog = SOMAXCONN) {
            if (listen(this->fd_, backlog) < 0) {
                throw SocketErrorException("Listen() failed." + std::string(" errno=") + std::to_string(errno) + " (" + std::string(strerror(errno)) + ")");
            }

            LOG(INFO) << "Listen(); S[fd=" << this->fd_ << "]";
        }

        void Connect(SA addr) {
            if (connect(this->fd_, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
                throw SocketErrorException("Connect() failed." + std::string(" errno=") + std::to_string(errno) + " (" + std::string(strerror(errno)) + ")");
            }

            LOG(INFO) << "Connect(); S[fd=" << this->fd_ << "] to (" << addr.ip() << ":" << addr.port()
                      << ")";
        }

        ssize_t Send(unsigned char *buf, size_t size, int flags) { // TODO merge with SendTo()
            ssize_t bytes_sent = 0;

            if((bytes_sent = send(this->fd_, buf, size - bytes_sent, flags)) < 0) { // TODO check bytes written
                throw SocketErrorException("Send() error." + std::string(" errno=") + std::to_string(errno) + " (" + std::string(strerror(errno)) + ")");
            }

            //LOG(INFO) << "Send() -> " << bytes_sent << "bytes; S[fd=" << this->fd_ << "]";

            return bytes_sent;
        }

        ssize_t Recv(unsigned char *buf, size_t size, int flags) { // TODO merge with RecvFrom()
            ssize_t bytes_read = 0;

            if ((bytes_read = recv(this->fd_, buf, size, flags)) < 0) { // TODO buffer size
                throw SocketErrorException("Recv() error." + std::string(" errno=") + std::to_string(errno) + " (" + std::string(strerror(errno)) + ")");
            }

            //LOG(INFO) << "Recv() -> " << bytes_read << "bytes; S[fd=" << this->fd_ << "]";

            return bytes_read;
        }

        ssize_t SendTo(unsigned char *buf, size_t size, int flags, SA dest_addr) {
            ssize_t bytes_sent = 0;

            if ((bytes_sent = sendto(this->fd_, buf, size - bytes_sent, flags, (struct sockaddr *) &dest_addr, sizeof(dest_addr))) < 0) { // TODO check bytes written
                throw SocketErrorException("SendTo() error." + std::string(" errno=") + std::to_string(errno) + " (" + std::string(strerror(errno)) + ")");
            }

            LOG(INFO) << "SendTo() S[fd=" << this->fd_ << "]";

            return bytes_sent;
        }

        ssize_t RecvFrom(unsigned char *buf, size_t size, int flags, SA src_addr) {
            ssize_t bytes_read = 0;
            socklen_t socklen = sizeof(sockaddr);

            if ((bytes_read = recvfrom(this->fd_, buf, size, flags, (struct sockaddr *) &src_addr, &socklen)) < 0) { // TODO check bytes written
                throw SocketErrorException("RecvFrom error." + std::string(" errno=") + std::to_string(errno) + " (" + std::string(strerror(errno)) + ")");
            }

            LOG(INFO) << "RecvFrom() S[fd=" << this->fd_ << "]";

            return bytes_read;
        }

        short Poll(short events, int timeout = 0) {
            pollfd poll_fd[1];
            poll_fd[0].fd = this->fd_;
            poll_fd[0].events = events;

            if (poll(poll_fd, 1, timeout) < 0) {
                throw SocketErrorException("Poll failed." + std::string(" errno=") + std::to_string(errno) + " (" + std::string(strerror(errno)) + ")");
            }

            //LOG(INFO) << "Poll(" << events << ", " << timeout << ") -> " << poll_fd[0].revents << ", S[fd=" << this->fd_ << "]";

            return poll_fd[0].revents;
        }

        SA GetSockName() {
            SA addr;
            socklen_t addrlen = sizeof(addr);

            if (getsockname(this->fd_, (struct ::sockaddr *) &addr, &addrlen) < 0) {
                throw SocketErrorException("GetSockName failed." + std::string(" errno=") + std::to_string(errno) + " (" + std::string(strerror(errno)) + ")");
            }

            LOG(INFO) << "GetSockName() -> " << addr.ip() << ":" << addr.port();

            return addr;
        }

        SA GetPeerName() {
            SA addr;
            socklen_t addrlen = sizeof(addr);

            if (getpeername(this->fd_, (struct ::sockaddr *) &addr, &addrlen) < 0) {
                throw SocketErrorException("GetSockName failed." + std::string(" errno=") + std::to_string(errno) + " (" + std::string(strerror(errno)) + ")");
            }

            LOG(INFO) << "GetPeerName() -> " << addr.ip() << ":" << addr.port();

            return addr;
        }

        template<typename T>
        void SetSockOpt(int level, int optname, T optval) {
            if (setsockopt(this->fd_, level, optname, (void *) &optval, sizeof(T)) < 0) {
                throw SocketErrorException("SetSockOpt failed." + std::string(" errno=") + std::to_string(errno) + " (" + std::string(strerror(errno)) + ")");
            }

            LOG(INFO) << "SetSockOpt(" << level << ", " << optname << ", " << optval << ")";
        }

        void Close() {
            close(this->fd_);

            LOG(INFO) << "Close() S[fd=" << this->fd_ << "]";
        }

        virtual ~Socket() {
            this->Close();
        }

    protected:
        Socket(int domain, int type, int protocol) {
            if ((this->fd_ = socket(domain, type, protocol)) < 0) {
                throw SocketErrorException("Connect failed." + std::string(" errno=") + std::to_string(errno) + " (" + std::string(strerror(errno)) + ")");
            }

            LOG(INFO) << "socket(domain=" << domain << ", type=" << type << ", protocol=" << protocol << ")" << " S[fd="
                      << this->fd_ << "]";
        }

        Socket(int fd) {
            this->fd_ = fd;

            LOG(INFO) << "socket(sockfd=" << fd << ")" << " S[fd=" << this->fd_ << "]";
        }

    private:
        int fd_ = -1;
        //std::mutex sendMutex_, recvMutex_;

        Socket() {}
    };

} // net


#endif //MULTIPATH_PROXY_SOCKET_H
