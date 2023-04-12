//
// Created by Matthias Hofstätter on 03.04.23.
//

#ifndef MULTIPATH_PROXY_LOCAL_H
#define MULTIPATH_PROXY_LOCAL_H

#include <list>
#include <thread>
#include "../net/TcpListener.h"
#include "Base.h"


namespace base {

    class Local : public Base {
    private:
        net::ipv4::TcpListener listener_;
        std::list<net::ipv4::TcpConnection> connections_;
        std::thread tProxy_, tSat_, tTer;

    public:
        Local(net::ipv4::TcpListener proxy, net::ipv4::TcpConnection ter, net::ipv4::TcpConnection sat);
    };

} // local


#endif //MULTIPATH_PROXY_LOCAL_H
