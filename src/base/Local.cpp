//
// Created by Matthias Hofstätter on 03.04.23.
//

#include "Local.h"

#include "glog/logging.h"


namespace base {
    Local::Local(net::ipv4::TcpListener proxy, net::ipv4::TcpConnection *ter, net::ipv4::TcpConnection *sat) : Base(ter, sat),
                                                                                                                proxy_(proxy) {
    }

    Local::~Local() {
        this->proxy_.Close();
    }
} // mpp