//
// Created by Matthias Hofstätter on 13.04.23.
//

#ifndef MULTIPATH_PROXY_HEARTBEATHEADER_H
#define MULTIPATH_PROXY_HEARTBEATHEADER_H

#include <string>
#include "Header.h"

namespace packet {

        struct HeartBeatHeader : Header {
            HeartBeatHeader();

            std::string ToString();
        };

} // packet

#endif //MULTIPATH_PROXY_HEARTBEATHEADER_H
