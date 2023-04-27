//
// Created by Matthias Hofstätter on 11.04.23.
//

#ifndef MULTIPATH_PROXY_BASE_H
#define MULTIPATH_PROXY_BASE_H

#include "../collections/FlowMap.h"

namespace net {
    namespace ipv4 {
        class TcpConnection;
    }

    class Bond;
}

/*namespace collections {
    class FlowMap;
}*/

namespace base {

    class Base {
    public:
        Base(net::ipv4::TcpConnection *ter, net::ipv4::TcpConnection *sat);

        virtual ~Base();

    protected:
        collections::FlowMap flowMap_;
        net::Bond *bond_;
    };
}


#endif //MULTIPATH_PROXY_PACKET_H
