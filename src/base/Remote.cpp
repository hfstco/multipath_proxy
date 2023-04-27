//
// Created by Matthias Hofstätter on 03.04.23.
//

#include "Remote.h"

namespace base {

    Remote::Remote(net::ipv4::TcpListener ter, net::ipv4::TcpListener sat) : Base(ter.Accept(), sat.Accept()) {
    }

    Remote::~Remote() {

    }

} // mpp::remote