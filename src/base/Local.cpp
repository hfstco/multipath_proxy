//
// Created by Matthias Hofstätter on 03.04.23.
//

#include "Local.h"

#include "glog/logging.h"

namespace base {
    Local::Local(net::ipv4::TcpListener listener, net::ipv4::TcpConnection ter, net::ipv4::TcpConnection sat) : listener_(listener), Base(ter, sat) {
    }
} // mpp