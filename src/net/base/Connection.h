//
// Created by Matthias Hofstätter on 04.03.23.
//

#ifndef MULTIPATH_PROXY_CONNECTION_H
#define MULTIPATH_PROXY_CONNECTION_H

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
            return S::Send(data, size, flags);
        };

        short Poll(short events, int timeout = 0) {
            return S::Poll(events, timeout);
        }

        SA GetSockName() {
            return S::GetSockName();
        }

        SA GetPeerName() {
            return S::GetSockName();
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
