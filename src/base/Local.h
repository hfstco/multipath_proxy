//
// Created by Matthias Hofstätter on 03.04.23.
//

#ifndef MULTIPATH_PROXY_LOCAL_H
#define MULTIPATH_PROXY_LOCAL_H

#include "Base.h"

namespace net {
    class Proxy;

    namespace ipv4 {
        class TcpConnection;
        class TcpListener;
        class SockAddr_In;
    }
}

namespace base {

    class Local : public Base {
    public:
        Local(net::ipv4::TcpConnection *ter, net::ipv4::TcpConnection *sat, net::ipv4::SockAddr_In sockAddrInProxy);

        virtual ~Local();

    private:
        net::Proxy *proxy_;
    };

} // local


#endif //MULTIPATH_PROXY_LOCAL_H
