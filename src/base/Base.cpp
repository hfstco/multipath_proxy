//
// Created by Matthias Hofstätter on 11.04.23.
//

#include "Base.h"


namespace base {

    Base::Base(net::ipv4::TcpConnection ter, net::ipv4::TcpConnection sat) : ter_(ter), sat_(sat) {

    }
}
