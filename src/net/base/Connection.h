//
// Created by Matthias Hofstätter on 04.03.23.
//

#ifndef MULTIPATH_PROXY_CONNECTION_H
#define MULTIPATH_PROXY_CONNECTION_H

#include <sys/ioctl.h>
#include <mutex>
#include <glog/logging.h>

#include "TcpSocket.h"

namespace net {

    template <IsSocket S, IsSockAddr SA>
    class Listener;

    template <IsSocket S, IsSockAddr SA>
    class Connection : protected S {
        friend Listener<S, SA>;

    public:
        ssize_t Recv(unsigned char *data, size_t size, int flags) {
            return S::Recv(data, size, flags);
        };

        ssize_t Send(unsigned char *data, size_t size, int flags) {
            return S::Send(data, size, flags | MSG_NOSIGNAL);
        };

        short Poll(short events, int timeout = 0) {
            return S::Poll(events, timeout);
        }

        SA GetSockName() {
            return S::GetSockName();
        }

        SA GetPeerName() {
            return S::GetPeerName();
        }

        size_t GetOutQueueSize() {
            size_t queueSize = 0;

            if( ioctl(S::fd(), TIOCOUTQ, &queueSize) < 0 ) {
                throw SocketErrorException("ioctl(); errno=" + std::to_string(errno) + " (" + std::string(strerror(errno)) + ")");
            }

            return queueSize;
        }

        size_t GetInQueueSize() {
            size_t queueSize = 0;

            if( ioctl(S::fd(), FIONREAD, &queueSize) < 0 ) {
                throw SocketErrorException("ioctl(); errno=" + std::to_string(errno) + " (" + std::string(strerror(errno)) + ")");
            }

            return queueSize;
        }

        std::string ToString() {
            /*std::stringstream stringStream;
            stringStream << "Connection[fd=" << std::to_string(S::fd()) << ", sockName=";
            try {
                stringStream << S::GetSockName().ToString();
            } catch (SocketErrorException e) {
                stringStream << e.ToString();
            }
            stringStream << ", peerAddr=";
            try {
                stringStream << S::GetPeerName().ToString();
            } catch (SocketErrorException e) {
                stringStream << e.ToString();
            }
            stringStream << "]";
            return stringStream.str();*/
            return "Connection[fd=" + std::to_string(S::fd()) + "]";
        }

        void Close() {
            S::Close();
        }

        void Shutdown(int how) {
            S::Shutdown(how);
        }

        virtual ~Connection() {
            DLOG(INFO) << ToString() << ".~Connection()";
        }

    protected:
        Connection(S socket) : S(socket) {
            DLOG(INFO) << "Connection(socket=" << S::ToString() << ") * " << ToString();

            S::SetSockOpt(SOL_SOCKET, SO_REUSEADDR, 1);
        }

        Connection(SA peeraddr) : Connection() {
            DLOG(INFO) << "Connection(peeraddr=" << peeraddr.ToString() << ") * " << ToString();

            S::Connect(peeraddr);
        }

        Connection(SA peeraddr, SA sockaddr) : Connection() {
            DLOG(INFO) << "Connection(peeraddr=" << peeraddr.ToString() << ", sockaddr=" << sockaddr.ToString() << ") * " << ToString();

            S::Bind(peeraddr);
            S::Connect(peeraddr);
        };

    private:
        Connection() {
            S::SetSockOpt(SOL_SOCKET, SO_REUSEADDR, 1);
        };
    };

}


#endif //MULTIPATH_PROXY_CONNECTION_H
