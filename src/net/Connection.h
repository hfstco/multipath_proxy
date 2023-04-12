//
// Created by Matthias Hofstätter on 04.03.23.
//

#ifndef MULTIPATH_PROXY_CONNECTION_H
#define MULTIPATH_PROXY_CONNECTION_H

#include "TcpSocket.h"
#include "UdpSocket.h"

namespace net {

    template <class S, class SA>
    class Listener;

    template <class S, class SA>
    class Connection : public S {
        friend Listener<S, SA>;

    public:
        std::vector<char> Recv(int flags) {
            return S::Recv(flags);
        };

        void Send(std::vector<char> buf, int flags) {
            S::Send(buf, flags);
        };

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
