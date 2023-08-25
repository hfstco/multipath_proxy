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
        template<IsSocket AS>
        AS *Accept(SA &addr) {
            /*S *s = S::Accept(addr);
            Connection<S, SA> *c = new Connection<S, SA>(s);
            delete s;
            return c;*/
            return  S::template Accept<AS>(addr);
        }

        template<IsSocket AS>
        AS *Accept() {
            /*S *s = S::Accept();
            Connection<S, SA> *c = new Connection<S, SA>(s);
            delete s;
            return c;*/
            return  S::template Accept<AS>();
        }

        std::string ToString() {
            return "Listener[fd=" + std::to_string(S::fd()) + ", sockName=" + S::GetSockName().to_string() + "]";
        }

        void Close() {
            S::Close();
        }

        virtual ~Listener() {
            DLOG(INFO) << ToString() << ".~Listener()";
        }

    protected:
        Listener(SA sockaddr) : Listener() {
            DLOG(INFO) << "Listener(sockaddr=" << sockaddr.to_string() << ") * " << ToString();

            S::Bind(sockaddr);
            S::Listen();
        }

    private:
        context::Context *context_ = &context::Context::GetDefaultContext();

        Listener() {
            S::SetSockOpt(SOL_SOCKET, SO_REUSEADDR, 1);
        }
    };
}


#endif //MULTIPATH_PROXY_LISTENER_H
