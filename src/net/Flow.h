//
// Created by Matthias Hofstätter on 14.04.23.
//

#ifndef MULTIPATH_PROXY_FLOW_H
#define MULTIPATH_PROXY_FLOW_H

#include <atomic>
#include <thread>

#include "../collections/FlowPacketQueue.h"

namespace packet {
    class FlowPacket;
}

namespace net {

    namespace ipv4 {
        class TcpConnection;
    }

    class Bond;

    class Flow {
    public:
        static Flow *make(net::ipv4::TcpConnection *pTcpConnection, net::Bond *pBond);

        void WriteToFlow(packet::FlowPacket *pFlowPacket);

        virtual ~Flow();

    protected:
        Flow(net::ipv4::TcpConnection *pTcpConnection, net::Bond *pBond);

    private:
        net::ipv4::TcpConnection *connection_;

        collections::FlowPacketQueue tx_; // outgoing packages
        collections::FlowPacketQueue rx_; // incoming packages

        uint16_t rxId_;

        std::atomic_bool stop_; // TODO implement Handler
        std::thread *recvFromConnectionHandler_; // TODO implement Handler
        std::thread *sendToConnectionHandler_; // TODO implement Handler
        std::thread *sendToBondHandler_; // TODO implement Handler

        net::Bond *bond_;

        void RecvFromConnection();
        void SendToConnection();

        void SendToBond();
    }; // Flow

} // net

#endif //MULTIPATH_PROXY_FLOW_H
