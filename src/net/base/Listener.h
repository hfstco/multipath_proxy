//
// Created by Matthias Hofstätter on 31.03.23.
//

#ifndef MULTIPATH_PROXY_LISTENER_H
#define MULTIPATH_PROXY_LISTENER_H

#include <netinet/in.h>
#include "Socket.h"
#include "SockAddr.h"
#include "Connection.h"


namespace net {

    template <IsSocket S, IsSockAddr SA>
    class Listener : public S {
        friend Connection<S, SA>;

    public:
        Connection<S, SA> *Accept(SA &addr) {
            return (Connection<S, SA> *)S::Accept(addr);
        }

        Connection<S, SA> *Accept() {
            return (Connection<S, SA> *)S::Accept();
        }

        void Close() {
            S::Close();
        }

    protected:
        Listener() {
            S::SetSockOpt(SOL_SOCKET, SO_REUSEADDR, 1);
            //S::SetSockOpt(SOL_IP, IP_TRANSPARENT, 1); TODO
        }

        Listener(S socket) : S(socket) {}

        Listener(SA sockaddr) : Listener() {
            S::Bind(sockaddr);
            S::Listen();
        }
    };
}


#endif //MULTIPATH_PROXY_LISTENER_H
