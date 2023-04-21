//
// Created by Matthias Hofstätter on 03.04.23.
//

#ifndef MULTIPATH_PROXY_REMOTE_H
#define MULTIPATH_PROXY_REMOTE_H

#include <thread>
#include "Base.h"
#include "../net/base/TcpListener.h"


namespace base {

    class Remote : public Base {
    private:
        std::thread tSat_, tTer;

    public:
        Remote(net::ipv4::TcpListener ter, net::ipv4::TcpListener sat);
    };

} // remote


#endif //MULTIPATH_PROXY_REMOTE_H
