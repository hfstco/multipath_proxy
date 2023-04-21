//
// Created by Matthias Hofstätter on 13.04.23.
//

#ifndef MULTIPATH_PROXY_HEARTBEATHEADER_H
#define MULTIPATH_PROXY_HEARTBEATHEADER_H

#include "Header.h"

namespace packet {
    namespace header {

        struct HeartBeatHeader : Header {
            HeartBeatHeader() : Header(TYPE::HEARTBEAT) {}
        };

    } // header
} // packet

#endif //MULTIPATH_PROXY_HEARTBEATHEADER_H
