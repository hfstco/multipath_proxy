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
#include <sys/ioctl.h>

#include "../exception/Exception.h"
#include "SockAddr.h"

namespace net {

    class Flow;

    namespace ipv4 {
        class TcpSocket;
        class TcpConnection;
    }
    namespace ipv6 {
        class TcpSocket;
        class TcpConnection;
    }

    template<class S>
    concept IsSocket = std::same_as<ipv4::TcpSocket, S> || std::same_as<ipv6::TcpSocket, S> || std::same_as<int, S> || std::same_as<ipv4::TcpConnection, S> || std::same_as<ipv6::TcpConnection, S> || std::same_as<Flow, S>;

    template<class SA>
    concept IsSockAddr = std::same_as<SockAddr, SA> || std::same_as<ipv4::SockAddr_In, SA> || std::same_as<ipv6::SockAddr_In6, SA> || std::same_as<sockaddr*, SA>; // TODO

    template<IsSocket S = int, IsSockAddr SA = sockaddr*> // test default socket
    class Socket {

    public:
        Socket() = delete;

        static S *make(int domain, int type, int protocol) {
            return new Socket(domain, type, protocol);
        }

        int fd() const {
            return fd_;
        }

        void Bind(SA addr) {
            if (bind(fd_, (struct sockaddr *) &addr, sizeof(SA)) < 0) {
                SocketErrorException socketErrorException = SocketErrorException(std::string(strerror(errno)));

                VLOG(3) << to_string() << ".Bind(" << addr.to_string() << ") ! " << socketErrorException.ToString();

                throw socketErrorException;
            }

            //VLOG(3) << to_string() << ".Bind(" << addr.to_string() << ")";
        }

        template<IsSocket AS = S>
        AS *Accept() { // TODO merge with accept(SA &sockaddr)
            int fd = -1;

            if ((fd = accept(fd_, NULL, NULL)) < 0) {
                SocketErrorException socketErrorException = SocketErrorException(std::string(strerror(errno)));

                VLOG(3) << to_string() << ".accept() ! " << socketErrorException.ToString();

                throw socketErrorException;
            }

            AS *s = new AS(fd);

            //VLOG(3) << to_string() << ".accept() -> " << s->to_string();

            return s;
        };

        template<IsSocket AS = S>
        AS *Accept(SA &addr) {
            int fd = -1;
            socklen_t addrlen = sizeof(addr);

            if ((fd = accept(fd_, (struct sockaddr *) &addr, &addrlen)) < 0) {
                SocketErrorException socketErrorException = SocketErrorException(std::string(strerror(errno)));

                VLOG(3) << to_string() << ".accept(" << addr.to_string() << ") ! " << socketErrorException.ToString();

                throw socketErrorException;
            }

            AS* s = new AS(fd);

            //VLOG(3) << to_string() << ".accept(" << addr.to_string() << ") -> " << s->to_string();

            return s;
        };

        void Listen(int backlog = SOMAXCONN) {
            if (listen(fd_, backlog) < 0) {
                SocketErrorException socketErrorException = SocketErrorException(std::string(strerror(errno)));

                VLOG(3) << to_string() << ".Listen() ! " << socketErrorException.ToString();

                throw socketErrorException;
            }

            //VLOG(3) << to_string() << ".Listen()";
        }

        void Connect(SA addr) {
            if (connect(fd_, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
                SocketErrorException socketErrorException = SocketErrorException(std::string(strerror(errno)));

                VLOG(3) << to_string() << ".Connect(" << addr.to_string() << ") ! " << socketErrorException.ToString();

                throw socketErrorException;
            }

            //VLOG(3) << to_string() << ".Connect(" << addr.to_string() << ")";
        }

        ssize_t Send(unsigned char *buf, size_t size, int flags) { // TODO merge with SendTo()
            ssize_t bytes_sent = 0;

            if((bytes_sent = send(fd_, buf, size - bytes_sent, flags)) < 0) { // TODO check bytes written
                SocketErrorException socketErrorException = SocketErrorException(std::string(strerror(errno)));

                VLOG(3) << to_string() << ".Send(buf=" << buf << ", size=" << size << ", flags=" << flags << ") ! " << socketErrorException.ToString();

                throw socketErrorException;
            }

            //VLOG(3) << to_string() << ".Send(buf=" << buf << ", size=" << size << ", flags=" << flags << ") -> " << bytes_sent << "recvBytes";

            return bytes_sent;
        }

        ssize_t Recv(unsigned char *buf, size_t size, int flags) { // TODO merge with RecvFrom()
            ssize_t bytes_read = 0;

            if ((bytes_read = recv(fd_, buf, size, flags)) < 0) { // TODO buffer size
                SocketErrorException socketErrorException = SocketErrorException(std::string(strerror(errno)));

                VLOG(3) << to_string() << ".Recv(buf=" << buf << ", size=" << size << ", flags=" << flags << ") ! " << socketErrorException.ToString();

                throw socketErrorException;
            }

            //VLOG(3) << to_string() << ".Recv(buf=" << buf << ", size=" << size << ", flags=" << flags << ") -> " << bytes_read << "recvBytes";

            return bytes_read;
        }

        ssize_t SendTo(unsigned char *buf, size_t size, int flags, SA dest_addr) {
            ssize_t bytes_sent = 0;

            if ((bytes_sent = sendto(this->fd_, buf, size - bytes_sent, flags, (struct sockaddr *) &dest_addr, sizeof(dest_addr))) < 0) { // TODO check bytes written
                SocketErrorException socketErrorException = SocketErrorException(std::string(strerror(errno)));

                VLOG(3) << to_string() << ".SendTo(buf=" << buf << ", size=" << size << ", flags=" << flags << ", SA=" << dest_addr.to_string() << ") ! " << socketErrorException.ToString();

                throw socketErrorException;
            }

            //VLOG(3) << to_string() << ".SendTo(buf=" << buf << ", size=" << size << ", flags=" << flags << ", SA=" << dest_addr.to_string() << ") -> " << bytes_sent << "recvBytes";

            return bytes_sent;
        }

        ssize_t RecvFrom(unsigned char *buf, size_t size, int flags, SA src_addr) {
            ssize_t bytes_read = 0;
            socklen_t socklen = sizeof(sockaddr);

            if ((bytes_read = recvfrom(fd_, buf, size, flags, (struct sockaddr *) &src_addr, &socklen)) < 0) { // TODO check bytes written
                SocketErrorException socketErrorException = SocketErrorException(std::string(strerror(errno)));

                VLOG(3) << to_string() << ".RecvFrom(buf=" << buf << ", size=" << size << ", flags=" << flags << ", SA=" << src_addr.to_string() << ") ! " << socketErrorException.ToString();

                throw socketErrorException;
            }

            //VLOG(3) << to_string() << ".RecvFrom(buf=" << buf << ", size=" << size << ", flags=" << flags << ", SA=" << src_addr.to_string() << ") -> " << bytes_read << "recvBytes";

            return bytes_read;
        }

        short Poll(short events, int timeout = 0) {
            pollfd fds[1];
            fds[0].fd = fd_;
            fds[0].events = events;

            try {
                Socket::Poll(fds, 1, timeout);
            } catch (Exception e) {

            }

            //VLOG(3) << to_string() << ".Poll(" << events << ", " << timeout << ") -> " << fds[0].revents;

            return fds[0].revents;
        }

        static void Poll(pollfd *fds, int nfds, int timeout = 0) {
            if (poll(fds, nfds, timeout) < 0) {
                SocketErrorException socketErrorException = SocketErrorException(std::string(strerror(errno)));

                VLOG(3) << "Socket::Poll(fds=" << fds << ", nfds=" << nfds << ", timeout=" << timeout << ") ! " << socketErrorException.ToString();

                throw socketErrorException;
            }

            //VLOG(3) << "Socket::Poll(fds=" << fds << ", nfds=" << nfds << ", timeout=" << timeout << ")";
        }

        SA GetSockName() {
            SA addr;
            socklen_t addrlen = sizeof(addr);

            if (getsockname(this->fd_, (struct ::sockaddr *) &addr, &addrlen) < 0) {
                SocketErrorException socketErrorException = SocketErrorException(std::string(strerror(errno)));

                VLOG(3) << to_string() << ".GetSockName() ! " << socketErrorException.ToString();

                throw socketErrorException;
            }

            //VLOG(3) << to_string() << ".GetSockName() -> " << addr.to_string();

            return addr;
        }

        SA GetPeerName() {
            SA addr;
            socklen_t addrlen = sizeof(addr);

            if (getpeername(this->fd_, (struct ::sockaddr *) &addr, &addrlen) < 0) {
                SocketErrorException socketErrorException = SocketErrorException("GetSockName failed." + std::string(" errno=") + std::to_string(errno) + " (" + std::string(strerror(errno)) + ")");

                VLOG(3) << to_string() << ".GetPeerName() ! " << socketErrorException.ToString();

                throw socketErrorException;
            }

            //VLOG(3) << to_string() << ".GetPeerName() -> " << addr.to_string();

            return addr;
        }

        template<typename T>
        void SetSockOpt(int level, int optname, T optval) {
            if (setsockopt(this->fd_, level, optname, &optval, sizeof(T)) < 0) {
                SocketErrorException socketErrorException = SocketErrorException(std::string(strerror(errno)));

                VLOG(3) << to_string() << ".SetSockOpt(level=" << level << ", optname=" << optname << ", optval=" << optval << ") ! " << socketErrorException.ToString();

                throw socketErrorException;
            }

            //VLOG(3) << to_string() << ".SetSockOpt(level=" << level << ", optname=" << optname << ", optval=" << optval << ")";
        }

        template<typename T>
        T GetSockOpt(int level, int optname) {
            T optval;
            socklen_t optlen = sizeof(T);
            if (getsockopt(this->fd_, level, optname, &optval, &optlen) < 0) {
                SocketErrorException socketErrorException = SocketErrorException(std::string(strerror(errno)));

                VLOG(3) << to_string() << ".GetSockOpt(level=" << level << ", optname=" << optname << ") ! " << socketErrorException.ToString();

                throw socketErrorException;
            }

            //VLOG(3) << to_string() << ".GetSockOpt(level=" << level << ", optname=" << optname << ") -> " << optval;

            return optval;
        }

        template<typename T>
        T IoCtl(unsigned long request) {
            T value;
            if (ioctl(fd_, request, &value) == -1) {
                SocketErrorException socketErrorException = SocketErrorException(std::string(strerror(errno)));

                VLOG(3) << to_string() << ".IoCtl(request=" << request << ") ! " << socketErrorException.ToString();

                throw socketErrorException;
            }

            return value;
        }

        void Close() {
            if (close(fd_) == -1) {
                SocketErrorException socketErrorException = SocketErrorException(std::string(strerror(errno)));

                VLOG(3) << to_string() << ".Close() ! " << socketErrorException.ToString();
            }

            //VLOG(3) << to_string() << ".Close()";
        }

        void Shutdown(int how) {
            std::stringstream stringStream;
            stringStream << to_string() << ".Shutdown(";
            switch (how) {
                case SHUT_RD:
                    stringStream << "SHUT_RD";
                    break;
                case SHUT_WR:
                    stringStream << "SHUT_WR";
                    break;
                case SHUT_RDWR:
                    stringStream << "SHUT_RDWR";
                    break;
            }
            stringStream << ")";

            if (shutdown(fd_, how) == -1) {
                SocketErrorException socketErrorException = SocketErrorException(std::string(strerror(errno)));

                VLOG(3) << stringStream.str() << " ! " << socketErrorException.ToString();

                throw socketErrorException;
            }

            //VLOG(3) << stringStream.str(); // TODO log
        }

        std::string to_string() {
            return "Socket[fd=" + std::to_string(fd()) + "]";
        }

        std::string ToRepresentation() {
            return "S[fd=" + std::to_string(fd()) + "]";
        }

        virtual ~Socket() {
            this->Close();

            DLOG(INFO) << to_string() << ".~Socket()";
        }

    protected:
        Socket(int domain, int type, int protocol) {
            if ((fd_ = socket(domain, type, protocol)) < 0) {
                SocketErrorException socketErrorException = SocketErrorException("Connect failed." + std::string(" errno=") + std::to_string(errno) + " (" + std::string(strerror(errno)) + ")");

                VLOG(3) << "Socket(domain=" << domain << ", type=" << type << ", protocol=" << protocol << ") ! " + socketErrorException.ToString();

                throw socketErrorException;
            }

            DLOG(INFO) << "Socket(domain=" << domain << ", type=" << type << ", protocol=" << protocol << ") * " +
                    to_string();
        }

        explicit Socket(int fd) {
            fd_ = fd;

            DLOG(INFO) << "Socket(fd=" << fd << ") * " << to_string();
        }

    private:
        int fd_;
    };

} // net


#endif //MULTIPATH_PROXY_SOCKET_H
