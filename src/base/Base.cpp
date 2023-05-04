//
// Created by Matthias Hofstätter on 11.04.23.
//

#include <glog/logging.h>

#include "Base.h"

#include "../net/Bond.h"

namespace base {

    Base::Base(net::ipv4::TcpConnection *ter, net::ipv4::TcpConnection *sat) : flowMap_(collections::FlowMap()), bond_(net::Bond::make(ter, sat, &flowMap_)) {
    }

    Base::~Base() {
        delete bond_;
    }

}
