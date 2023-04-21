//
// Created by Matthias Hofstätter on 03.04.23.
//

#ifndef MULTIPATH_PROXY_LOCAL_H
#define MULTIPATH_PROXY_LOCAL_H

#include <list>
#include <thread>
#include <queue>
#include "../net/base/TcpListener.h"
#include "Base.h"
#include "../handler/Handler.h"


namespace base {

    class Local : public Base {

    public:
        Local(net::ipv4::TcpListener proxy, net::ipv4::TcpConnection *ter, net::ipv4::TcpConnection *sat);

        virtual ~Local();

    private:
        net::ipv4::TcpListener proxy_;

    };

} // local


#endif //MULTIPATH_PROXY_LOCAL_H
