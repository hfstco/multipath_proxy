//
// Created by Matthias Hofstätter on 11.04.23.
//

#ifndef MULTIPATH_PROXY_BASE_H
#define MULTIPATH_PROXY_BASE_H

#include "../net/base/TcpConnection.h"
#include "../net/base/TcpListener.h"
#include "../collections/FlowMap.h"
#include "../net/Bond.h"

namespace base {

    class Base {

    public:
        Base(net::ipv4::TcpConnection *ter, net::ipv4::TcpConnection *sat) : bond_(ter, sat, &flowMap_) {}

        virtual ~Base() {}

    protected:
        net::Bond bond_;
        collections::FlowMap flowMap_ = collections::FlowMap();

    private:

    };
}


#endif //MULTIPATH_PROXY_PACKET_H
