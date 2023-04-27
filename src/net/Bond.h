//
// Created by Matthias Hofstätter on 18.04.23.
//

#ifndef MULTIPATH_PROXY_BOND_H
#define MULTIPATH_PROXY_BOND_H

#include <atomic>
#include <thread>

namespace packet {
    class FlowPacket;
}

namespace collections {
    class FlowMap;
}

namespace net {

    namespace ipv4 {
        class TcpConnection;
    }

    class Bond {
    public:
        static Bond *make(net::ipv4::TcpConnection *ter, net::ipv4::TcpConnection *sat, collections::FlowMap *flows);

        void WriteToBond(packet::FlowPacket *pFlowPacket);

        virtual ~Bond();

    private:
        net::ipv4::TcpConnection *ter_, *sat_;
        collections::FlowMap *flows_;

        std::atomic_bool stop_; // TODO implement Handler
        std::thread *readTerHandler_; // TODO implement Handler
        std::thread *readSatHandler_; // TODO implement Handler
        // std::thread *bondManager_; // TODO implement Handler

        Bond(net::ipv4::TcpConnection *ter, net::ipv4::TcpConnection *sat, collections::FlowMap *flows);

        void ReadFromConnection(net::ipv4::TcpConnection *connection);
    };

} // net

#endif //MULTIPATH_PROXY_BOND_H
