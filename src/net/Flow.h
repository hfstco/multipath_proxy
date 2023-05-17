//
// Created by Matthias Hofstätter on 14.04.23.
//

#ifndef MULTIPATH_PROXY_FLOW_H
#define MULTIPATH_PROXY_FLOW_H

#include <atomic>
#include <thread>

#include "../collections/FlowPacketQueue.h"
#include "base/SockAddr.h"

namespace packet {
    class FlowPacket;
}

namespace handler {
    class FlowHandler;
}

namespace collections {
    class FlowMap;
}

namespace net {

    namespace ipv4 {
        class TcpConnection;
    }

    class Bond;

    class Flow {
    public:
        static Flow *make(net::ipv4::SockAddr_In source, net::ipv4::SockAddr_In destination, net::ipv4::TcpConnection *pTcpConnection, collections::FlowMap *flows, net::Bond *bond);

        uint64_t byteSize();

        void WriteToFlow(packet::FlowPacket *pFlowPacket);

        void SendToBond();
        void RecvFromConnection();
        void SendToConnection();

        virtual ~Flow();

    protected:
        Flow(net::ipv4::SockAddr_In source, net::ipv4::SockAddr_In destination, net::ipv4::TcpConnection *pTcpConnection, collections::FlowMap *flows, net::Bond *bond);

    private:
        net::ipv4::TcpConnection *connection_;
        net::ipv4::SockAddr_In source_;
        net::ipv4::SockAddr_In destination_;
        net::Bond *bond_;
        collections::FlowMap *flows_;

        collections::FlowPacketQueue toConnectionQueue_;
        collections::FlowPacketQueue toBondQueue_;
        std::atomic<uint64_t> toBondId_;
    }; // Flow

} // net

#endif //MULTIPATH_PROXY_FLOW_H
