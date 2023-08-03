//
// Created by Matthias Hofstätter on 18.04.23.
//

#ifndef MULTIPATH_PROXY_BOND_H
#define MULTIPATH_PROXY_BOND_H

#include <atomic>
#include <thread>

#include "SockAddr.h"
#include "../worker/Looper.h"

namespace packet {
    class Packet;
    class FlowPacket;
}

namespace collections {
    class FlowMap;
}

namespace task {
    class ThreadPool;
}

namespace context {
    class Context;
}

namespace net {

    namespace ipv4 {
        class TcpConnection;
    }

    class Flow;

    class Bond {
    public:
        Bond(net::ipv4::TcpConnection *terConnection, net::ipv4::TcpConnection *satConnection, context::Context *context);

        void SendToTer(packet::FlowPacket *flowPacket);
        void SendToSat(packet::FlowPacket *flowPacket);

        std::string ToString();

        virtual ~Bond();

    private:
        std::mutex sendTerMutex_;
        std::mutex sendSatMutex_;

        net::ipv4::SockAddr_In terSockAddr_, satSockAddr_;
        net::ipv4::TcpConnection *terConnection_, *satConnection_;

        context::Context *context_;

        void RecvFromConnection(net::ipv4::TcpConnection *connection);
        void SendToConnection(net::ipv4::TcpConnection *connection, packet::Packet *packet);

        void WriteHeartBeatPacket();

        worker::Looper *readFromTerLooper_;
        worker::Looper *readFromSatLooper_;
        worker::Looper *heartbeatLooper_;
    };

} // net

#endif //MULTIPATH_PROXY_BOND_H
