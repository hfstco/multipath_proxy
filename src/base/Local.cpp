//
// Created by Matthias Hofstätter on 03.04.23.
//

#include "Local.h"

#include "../net/Proxy.h"
#include "../net/base/SockAddr.h"

namespace base {

    Local::Local(net::ipv4::TcpConnection *ter, net::ipv4::TcpConnection *sat, net::ipv4::SockAddr_In sockAddrInProxy) : Base(ter, sat), proxy_(net::Proxy::make(sockAddrInProxy, &flowMap_, bond_)) {

    }

    Local::~Local() {
        delete proxy_;
    }

} // mpp