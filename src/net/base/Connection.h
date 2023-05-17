//
// Created by Matthias Hofstätter on 04.03.23.
//

#ifndef MULTIPATH_PROXY_CONNECTION_H
#define MULTIPATH_PROXY_CONNECTION_H

#include <sys/ioctl.h>
#include <mutex>

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

        bool IsConnected() {
            return S::Poll(POLLHUP, 0) == 0;
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
            return "Connection[fd=" + std::to_string(S::fd()) + "]";
        }

        void Close() {
            S::Close();
        }

    protected:
        Connection(S socket) : S(socket) {}

        Connection(SA peeraddr) : Connection() {
            S::Connect(peeraddr);
        }

        Connection(SA peeraddr, SA sockaddr) : Connection() {
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
