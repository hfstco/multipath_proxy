//
// Created by Matthias Hofstätter on 31.03.23.
//

#ifndef MULTIPATH_PROXY_LISTENER_H
#define MULTIPATH_PROXY_LISTENER_H

#include "Socket.h"
#include "SockAddr.h"
#include "Connection.h"

namespace net {

    template <class S, class SA>
    class Listener : public S {

    public:
        Connection<S, SA> Accept() {
            S s = S::Accept();

            return Connection<S, SA>(s);
        }

        void Close() {
            S::Close();
        }

    protected:
        Listener() {
            S::SetSockOpt(SOL_SOCKET, SO_REUSEADDR, 1);
        }

        Listener(S socket) : S(socket) {}

        Listener(SA sockaddr) : Listener() {
            S::Bind(sockaddr);
            S::Listen();
        }
    };
}


#endif //MULTIPATH_PROXY_LISTENER_H
