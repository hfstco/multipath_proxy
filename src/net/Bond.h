//
// Created by Matthias Hofstätter on 18.04.23.
//

#ifndef MULTIPATH_PROXY_BOND_H
#define MULTIPATH_PROXY_BOND_H

#include <atomic>
#include <thread>

#include "../net/base/SockAddr.h"

namespace packet {
    class Packet;
    class FlowPacket;
}

namespace collections {
    class FlowMap;
}

namespace net {

    namespace ipv4 {
        class TcpConnection;
    }

    class Flow;

    class Bond {
    public:
        static Bond *make(net::ipv4::TcpConnection *terConnection, net::ipv4::TcpConnection *satConnection, collections::FlowMap *flows);

        void WriteToTer(packet::FlowPacket *flowPacket);
        void WriteToSat(packet::FlowPacket *flowPacket);
        void WriteHeartBeatPacket();

        void ReadFromTerConnection();
        void ReadFromSatConnection();

        void CheckBondBuffers();

        std::string ToString();

        virtual ~Bond();

    private:
        std::mutex createFlowMutex_;
        std::mutex writeSatMutex_;
        std::mutex writeTerMutex_;

        net::ipv4::SockAddr_In terSockAddr_, satSockAddr_;
        net::ipv4::TcpConnection *terConnection_, *satConnection_;
        std::atomic<bool> useSatConnection;

        collections::FlowMap *flows_;

        Bond(net::ipv4::TcpConnection *terConnection, net::ipv4::TcpConnection *satConnection, collections::FlowMap *flows);

        void ReadFromConnection(net::ipv4::TcpConnection *connection);
        void WriteToConnection(net::ipv4::TcpConnection *connection, packet::Packet *packet);
    };

} // net

#endif //MULTIPATH_PROXY_BOND_H
