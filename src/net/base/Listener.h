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
            return (Connection<S, SA> *)(S::Accept(addr));
        }

        Connection<S, SA> *Accept() {
            return (Connection<S, SA> *)(S::Accept());
        }

        std::string ToString() {
            return "Listener[fd=" + std::to_string(S::fd()) + ", sockName=" + S::GetSockName().ToString() + "]";
        }

        void Close() {
            S::Close();
        }

        virtual ~Listener() {
            DLOG(INFO) << ToString() << ".~Listener()";
        }

    protected:
        Listener() {
            DLOG(INFO) << "Listener() * " << ToString();

            S::SetSockOpt(SOL_SOCKET, SO_REUSEADDR, 1);
        }

        Listener(S socket) : S(socket) {
            DLOG(INFO) << "Listener(socket=" << S::ToString() << ") * " << ToString();
        }

        Listener(SA sockaddr) : Listener() {
            DLOG(INFO) << "Listener(sockaddr=" << sockaddr.ToString() << ") * " << ToString();

            S::Bind(sockaddr);
            S::Listen();
        }
    };
}


#endif //MULTIPATH_PROXY_LISTENER_H
