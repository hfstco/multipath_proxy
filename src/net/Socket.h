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

namespace net {

    template<class S = int, class SA = sockaddr*> // test default socket
    class Socket {

    public:
        Socket(int domain, int type, int protocol) {
            if ((this->fd_ = socket(domain, type, protocol)) < 0) {
                throw SocketError("Connect failed." + std::string(" errno=") + std::to_string(errno) + " (" + std::string(strerror(errno)) + ")");
            }

            LOG(INFO) << "socket(domain=" << domain << ", type=" << type << ", protocol=" << protocol << ")" << " S[fd="
                      << this->fd_ << "]";
        }

        void Bind(SA addr) {
            if (bind(this->fd_, (struct sockaddr *) &addr, sizeof(SA)) < 0) {
                throw SocketError("Bind failed." + std::string(" errno=") + std::to_string(errno) + " (" + std::string(strerror(errno)) + ")");
            }

            LOG(INFO) << "Bind() S[fd=" << this->fd_ << "] to (" << addr.ip() << ":" << addr.port() << ")";
        }

        S Accept() { // TODO merge with Accept(SA &sockaddr)
            int temp_fd = -1;

            LOG(INFO) << "Accept() S[fd=" << this->fd_ << "]";

            if ((temp_fd = accept(this->fd_, NULL, NULL)) < 0) {
                throw SocketError("Accept failed." + std::string(" errno=") + std::to_string(errno) + " (" + std::string(strerror(errno)) + ")");
            }

            return S(temp_fd);
        };

        S Accept(SA &addr) {
            int temp_fd = -1;
            socklen_t addrlen = sizeof(addr);

            LOG(INFO) << "Accept() S[fd=" << this->fd_ << "]";

            if ((temp_fd = accept(this->fd_, (struct sockaddr *) &addr, &addrlen)) < 0) {
                throw SocketError("Accept failed." + std::string(" errno=") + std::to_string(errno) + " (" + std::string(strerror(errno)) + ")");
            }

            return S(temp_fd);
        };

        void Listen(int backlog = SOMAXCONN) {
            if (listen(this->fd_, backlog) < 0) {
                throw SocketError("Listen failed." + std::string(" errno=") + std::to_string(errno) + " (" + std::string(strerror(errno)) + ")");
            }

            LOG(INFO) << "Listen() S[fd=" << this->fd_ << "]";
        }

        void Connect(SA addr) {
            if (connect(this->fd_, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
                throw SocketError("Connect failed." + std::string(" errno=") + std::to_string(errno) + " (" + std::string(strerror(errno)) + ")");
            }

            LOG(INFO) << "Connect() S[fd=" << this->fd_ << "] to (" << addr.ip() << ":" << addr.port()
                      << ")";
        }

        std::vector<char> Read() {
            std::vector<char> buf(1024); // TODO buffer size
            int bytes_read = 0;

            if ((bytes_read = read(this->fd_, &buf[0], buf.size())) < 0) { // TODO buffer size
                throw SocketError("Read error." + std::string(" errno=") + std::to_string(errno) + " (" + std::string(strerror(errno)) + ")");
            }

            buf.resize(bytes_read);

            LOG(INFO) << "Read() S[fd=" << this->fd_ << "]";

            return buf;
        }

        void Write(std::vector<char> buf) {
            int bytes_written = 0;

            while (bytes_written < buf.size()) {
                int bytes_returned = 0; // TODO combine =+ possible?
                if ((bytes_returned = write(this->fd_, &buf[bytes_written], buf.size() - bytes_written)) < 0) { // TODO check bytes written
                    throw SocketError("Write error." + std::string(" errno=") + std::to_string(errno) + " (" +
                                      std::string(strerror(errno)) + ")");
                }
                bytes_written += bytes_returned;
            }

            LOG(INFO) << "Write() S[fd=" << this->fd_ << "]";
        }

        void Send(std::vector<char> buf, int flags) {
            int bytes_sent = 0;

            while (bytes_sent < buf.size()) {
                int bytes_returned = 0;
                if((bytes_returned = send(this->fd_, &buf[bytes_sent], buf.size() - bytes_sent, flags)) < 0) { // TODO check bytes written
                    throw SocketError("Send error." + std::string(" errno=") + std::to_string(errno) + " (" + std::string(strerror(errno)) + ")");
                }
                bytes_sent += bytes_returned;
            }

            LOG(INFO) << "Send() S[fd=" << this->fd_ << "]";
        }

        std::vector<char> Recv(int flags) {
            std::vector<char> buf(1024); // TODO buffer size
            int bytes_read = 0;

            if ((bytes_read = recv(this->fd_, &buf[0], buf.size(), flags)) < 0) { // TODO buffer size
                throw SocketError("Recv error." + std::string(" errno=") + std::to_string(errno) + " (" + std::string(strerror(errno)) + ")");
            }

            buf.resize(bytes_read);

            LOG(INFO) << "Recv() S[fd=" << this->fd_ << "]";

            return buf;
        }

        void SendTo(std::vector<char> buf, int flags, SA dest_addr) {
            int bytes_sent = 0;

            while (bytes_sent < buf.size()) {
                int bytes_returned = 0;
                if ((bytes_returned = sendto(this->fd_, &buf[bytes_sent], buf.size() - bytes_sent, flags, (struct sockaddr *) &dest_addr, sizeof(dest_addr))) < 0) { // TODO check bytes written
                    throw SocketError("SendTo error." + std::string(" errno=") + std::to_string(errno) + " (" + std::string(strerror(errno)) + ")");
                }
                bytes_sent += bytes_returned;
            }

            LOG(INFO) << "SendTo() S[fd=" << this->fd_ << "]";
        }

        std::vector<char> RecvFrom(int flags, SA src_addr) {
            std::vector<char> buffer(1000); // TODO buffer size
            int bytes_read = 0;
            socklen_t socklen = sizeof(sockaddr);

            if ((bytes_read = recvfrom(this->fd_, &buffer[0], buffer.size(), flags, (struct sockaddr *) &src_addr, &socklen)) < 0) { // TODO check bytes written
                throw SocketError("RecvFrom error." + std::string(" errno=") + std::to_string(errno) + " (" + std::string(strerror(errno)) + ")");
            }

            buffer.resize(bytes_read);

            LOG(INFO) << "RecvFrom() S[fd=" << this->fd_ << "]";

            return buffer;
        }

        short Poll(short revents, int nfds, int timeout = 0) {
            pollfd poll_fd[2];
            poll_fd[0].fd = this->fd_;
            poll_fd[0].revents = revents;

            if (poll(poll_fd, nfds, timeout) < 0) {
                throw SocketError("Poll failed." + std::string(" errno=") + std::to_string(errno) + " (" + std::string(strerror(errno)) + ")");
            }

            return 0;
        }

        SA GetSockName() {
            SA addr;
            socklen_t addrlen = sizeof(addr);

            if (getsockname(this->fd_, (struct ::sockaddr *) &addr, &addrlen) < 0) {
                throw SocketError("GetSockName failed." + std::string(" errno=") + std::to_string(errno) + " (" + std::string(strerror(errno)) + ")");
            }

            return addr;
        }

        SA GetPeerName() {
            SA addr;
            socklen_t addrlen = sizeof(addr);

            if (getpeername(this->fd_, (struct ::sockaddr *) &addr, &addrlen) < 0) {
                throw SocketError("GetSockName failed." + std::string(" errno=") + std::to_string(errno) + " (" + std::string(strerror(errno)) + ")");
            }

            return addr;
        }

        template<typename T>
        void SetSockOpt(int level, int optname, T optval) {
            if (setsockopt(this->fd_, level, optname, (void *) &optval, sizeof(T)) < 0) {
                throw SocketError("SetSockOpt failed." + std::string(" errno=") + std::to_string(errno) + " (" + std::string(strerror(errno)) + ")");
            }
        }

        void Close() {
            close(this->fd_);

            LOG(INFO) << "Close() S[fd=" << this->fd_ << "]";
        }

    protected:
        Socket(int fd) {
            this->fd_ = fd;

            LOG(INFO) << "socket(sockfd=" << fd << ")" << " S[fd=" << this->fd_ << "]";
        }

    private:
        int fd_ = -1;

        Socket() {}
    };

} // net


#endif //MULTIPATH_PROXY_SOCKET_H
