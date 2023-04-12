//
// Created by Matthias Hofstätter on 11.04.23.
//

#ifndef MULTIPATH_PROXY_BASE_H
#define MULTIPATH_PROXY_BASE_H

#include "../net/TcpConnection.h"
#include "../net/TcpListener.h"


namespace base {

    class Base {

    public:
        Base(net::ipv4::TcpConnection ter, net::ipv4::TcpConnection sat);

    private:
        net::ipv4::TcpConnection ter_, sat_;
    };
}


#endif //MULTIPATH_PROXY_BASE_H
