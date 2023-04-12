//
// Created by Matthias Hofstätter on 03.04.23.
//

#ifndef MULTIPATH_PROXY_REMOTE_H
#define MULTIPATH_PROXY_REMOTE_H

#include "../net/TcpListener.h"
#include "Base.h"


namespace base {

    class Remote : public Base {
    private:

    public:
        Remote(net::ipv4::TcpListener ter, net::ipv4::TcpListener sat);
    };
} // remote


#endif //MULTIPATH_PROXY_REMOTE_H
