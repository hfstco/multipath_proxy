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
                SocketErrorException socketErrorException = SocketErrorException(std::string(strerror(errno)));

                VLOG(3) << ToString() << ".Bind(" << addr.ToString() << ") ! " << socketErrorException.ToString();

                throw socketErrorException;
            }

            VLOG(3) << ToString() << ".Bind(" << addr.ToString() << ")";
        }

        S *Accept() { // TODO merge with Accept(SA &sockaddr)
            int fd = -1;

            if ((fd = accept(this->fd_, NULL, NULL)) < 0) {
                SocketErrorException socketErrorException = SocketErrorException(std::string(strerror(errno)));

                VLOG(3) << ToString() << ".Accept() ! " << socketErrorException.ToString();

                throw socketErrorException;
            }

            S *s = new S(fd);

            VLOG(3) << ToString() << ".Accept() -> " << s->ToString();

            return s;
        };

        S *Accept(SA &addr) {
            int fd = -1;
            socklen_t addrlen = sizeof(addr);

            if ((fd = accept(this->fd_, (struct sockaddr *) &addr, &addrlen)) < 0) {
                SocketErrorException socketErrorException = SocketErrorException(std::string(strerror(errno)));

                VLOG(3) << ToString() << ".Accept(" << addr.ToString() << ") ! " << socketErrorException.ToString();

                throw socketErrorException;
            }

            S* s = new S(fd);

            VLOG(3) << ToString() << ".Accept(" << addr.ToString() << ") -> " << s->ToString();

            return s;
        };

        void Listen(int backlog = SOMAXCONN) {
            if (listen(this->fd_, backlog) < 0) {
                SocketErrorException socketErrorException = SocketErrorException(std::string(strerror(errno)));

                VLOG(3) << ToString() << ".Listen() ! " << socketErrorException.ToString();

                throw socketErrorException;
            }

            VLOG(3) << ToString() << ".Listen()";
        }

        void Connect(SA addr) {
            if (connect(this->fd_, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
                SocketErrorException socketErrorException = SocketErrorException(std::string(strerror(errno)));

                VLOG(3) << ToString() << ".Connect(" << addr.ToString() << ") ! " << socketErrorException.ToString();

                throw socketErrorException;
            }

            VLOG(3) << ToString() << ".Connect(" << addr.ToString() << ")";
        }

        ssize_t Send(unsigned char *buf, size_t size, int flags) { // TODO merge with SendTo()
            ssize_t bytes_sent = 0;

            if((bytes_sent = send(this->fd_, buf, size - bytes_sent, flags)) < 0) { // TODO check bytes written
                SocketErrorException socketErrorException = SocketErrorException(std::string(strerror(errno)));

                VLOG(3) << ToString() << ".Send(buf=" << buf << ", size=" << size << ", flags=" << flags << ") ! " << socketErrorException.ToString();

                throw socketErrorException;
            }

            VLOG(3) << ToString() << ".Send(buf=" << buf << ", size=" << size << ", flags=" << flags << ") -> " << bytes_sent << "bytes";

            return bytes_sent;
        }

        ssize_t Recv(unsigned char *buf, size_t size, int flags) { // TODO merge with RecvFrom()
            ssize_t bytes_read = 0;

            if ((bytes_read = recv(this->fd_, buf, size, flags)) < 0) { // TODO buffer size
                SocketErrorException socketErrorException = SocketErrorException(std::string(strerror(errno)));

                VLOG(3) << ToString() << ".Recv(buf=" << buf << ", size=" << size << ", flags=" << flags << ") ! " << socketErrorException.ToString();

                throw socketErrorException;
            }

            VLOG(3) << ToString() << ".Recv(buf=" << buf << ", size=" << size << ", flags=" << flags << ") -> " << bytes_read << "bytes";

            return bytes_read;
        }

        ssize_t SendTo(unsigned char *buf, size_t size, int flags, SA dest_addr) {
            ssize_t bytes_sent = 0;

            if ((bytes_sent = sendto(this->fd_, buf, size - bytes_sent, flags, (struct sockaddr *) &dest_addr, sizeof(dest_addr))) < 0) { // TODO check bytes written
                SocketErrorException socketErrorException = SocketErrorException(std::string(strerror(errno)));

                VLOG(3) << ToString() << ".SendTo(buf=" << buf << ", size=" << size << ", flags=" << flags << ", SA=" << dest_addr.ToString() << ") ! " << socketErrorException.ToString();

                throw socketErrorException;
            }

            VLOG(3) << ToString() << ".SendTo(buf=" << buf << ", size=" << size << ", flags=" << flags << ", SA=" << dest_addr.ToString() << ") -> " << bytes_sent << "bytes";

            return bytes_sent;
        }

        ssize_t RecvFrom(unsigned char *buf, size_t size, int flags, SA src_addr) {
            ssize_t bytes_read = 0;
            socklen_t socklen = sizeof(sockaddr);

            if ((bytes_read = recvfrom(this->fd_, buf, size, flags, (struct sockaddr *) &src_addr, &socklen)) < 0) { // TODO check bytes written
                SocketErrorException socketErrorException = SocketErrorException(std::string(strerror(errno)));

                VLOG(3) << ToString() << ".RecvFrom(buf=" << buf << ", size=" << size << ", flags=" << flags << ", SA=" << src_addr.ToString() << ") ! " << socketErrorException.ToString();

                throw socketErrorException;
            }

            VLOG(3) << ToString() << ".RecvFrom(buf=" << buf << ", size=" << size << ", flags=" << flags << ", SA=" << src_addr.ToString() << ") -> " << bytes_read << "bytes";

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

            VLOG(3) << ToString() << ".Poll(" << events << ", " << timeout << ") -> " << fds[0].revents;

            return fds[0].revents;
        }

        static void Poll(pollfd *fds, int nfds, int timeout = 0) {
            if (poll(fds, nfds, timeout) < 0) {
                SocketErrorException socketErrorException = SocketErrorException(std::string(strerror(errno)));

                VLOG(3) << "Socket::Poll(fds=" << fds << ", nfds=" << nfds << ", timeout=" << timeout << ") ! " << socketErrorException.ToString();

                throw socketErrorException;
            }

            VLOG(3) << "Socket::Poll(fds=" << fds << ", nfds=" << nfds << ", timeout=" << timeout << ")";
        }

        SA GetSockName() {
            SA addr;
            socklen_t addrlen = sizeof(addr);

            if (getsockname(this->fd_, (struct ::sockaddr *) &addr, &addrlen) < 0) {
                SocketErrorException socketErrorException = SocketErrorException(std::string(strerror(errno)));

                VLOG(3) << ToString() << ".GetSockName() ! " << socketErrorException.ToString();

                throw socketErrorException;
            }

            VLOG(3) << ToString() << ".GetSockName() -> " << addr.ToString();

            return addr;
        }

        SA GetPeerName() {
            SA addr;
            socklen_t addrlen = sizeof(addr);

            if (getpeername(this->fd_, (struct ::sockaddr *) &addr, &addrlen) < 0) {
                SocketErrorException socketErrorException = SocketErrorException("GetSockName failed." + std::string(" errno=") + std::to_string(errno) + " (" + std::string(strerror(errno)) + ")");

                VLOG(3) << ToString() << ".GetPeerName() ! " << socketErrorException.ToString();

                throw socketErrorException;
            }

            VLOG(3) << ToString() << ".GetPeerName() -> " << addr.ToString();

            return addr;
        }

        template<typename T>
        void SetSockOpt(int level, int optname, T optval) {
            if (setsockopt(this->fd_, level, optname, (void *) &optval, sizeof(T)) < 0) {
                SocketErrorException socketErrorException = SocketErrorException(std::string(strerror(errno)));

                VLOG(3) << ToString() << ".SetSockOpt(level=" << level << ", optname=" << optname << ", optval=" << optval << ") ! " << socketErrorException.ToString();

                throw socketErrorException;
            }

            VLOG(3) << ToString() << ".SetSockOpt(level=" << level << ", optname=" << optname << ", optval=" << optval << ")";
        }

        template<typename T>
        T GetSockOpt(int level, int optname) {
            T optval;
            socklen_t optlen = sizeof(T);
            if (getsockopt(this->fd_, level, optname, (void *) &optval, &optlen) < 0) {
                SocketErrorException socketErrorException = SocketErrorException(std::string(strerror(errno)));

                VLOG(3) << ToString() << ".GetSockOpt(level=" << level << ", optname=" << optname << ", optval=" << optval << ") ! " << socketErrorException.ToString();

                throw socketErrorException;
            }

            VLOG(3) << ToString() << ".GetSockOpt(level=" << level << ", optname=" << optname << ", optval=" << optval << ") -> " << optval;

            return optval;
        }

        void Close() {
            if (close(fd_) == -1) {
                SocketErrorException socketErrorException = SocketErrorException(std::string(strerror(errno)));

                VLOG(3) << ToString() << ".Close() ! " << socketErrorException.ToString();
            }

            VLOG(3) << ToString() << ".Close()";
        }

        void Shutdown(int how) {
            std::stringstream stringStream;
            stringStream << ToString() << ".Shutdown(";
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

            VLOG(3) << stringStream.str(); // TODO log
        }

        std::string ToString() {
            return "Socket[fd=" + std::to_string(fd()) + "]";
        }

        std::string ToRepresentation() {
            return "S[fd=" + std::to_string(fd()) + "]";
        }

        virtual ~Socket() {
            this->Close();

            DLOG(INFO) << ToString() << ".~Socket()";
        }

    protected:
        Socket(int domain, int type, int protocol) {
            if ((this->fd_ = socket(domain, type, protocol)) < 0) {
                SocketErrorException socketErrorException = SocketErrorException("Connect failed." + std::string(" errno=") + std::to_string(errno) + " (" + std::string(strerror(errno)) + ")");

                VLOG(3) << "Socket(domain=" << domain << ", type=" << type << ", protocol=" << protocol << ") ! " + socketErrorException.ToString();

                throw socketErrorException;
            }

            DLOG(INFO) << "Socket(domain=" << domain << ", type=" << type << ", protocol=" << protocol << ") * " + ToString();
        }

        Socket(int fd) {
            fd_ = fd;

            DLOG(INFO) << "Socket(fd=" << fd << ") * " << ToString();
        }

    private:
        int fd_;
        //std::mutex sendMutex_, recvMutex_;

        Socket() : fd_(-1) {}
    };

} // net


#endif //MULTIPATH_PROXY_SOCKET_H
