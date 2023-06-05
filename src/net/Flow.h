//
// Created by Matthias Hofstätter on 14.04.23.
//

#ifndef MULTIPATH_PROXY_FLOW_H
#define MULTIPATH_PROXY_FLOW_H

#include <atomic>
#include <thread>

#include "../collections/BlockingFlowPacketQueue.h"
#include "base/SockAddr.h"
#include "../worker/Looper.h"

namespace packet {
    class FlowPacket;
}

namespace handler {
    class FlowHandler;
}

namespace collections {
    class FlowMap;
}

namespace context {
    class Context;
}

namespace task {
    class ThreadPool;
}

namespace net {

    namespace ipv4 {
        class TcpConnection;
    }

    class Bond;

    class Flow {
    public:
        static Flow *make(net::ipv4::SockAddr_In source, net::ipv4::SockAddr_In destination, net::ipv4::TcpConnection *pTcpConnection, net::Bond *bond, context::Context *context);

        uint64_t byteSize();

        void WriteToFlow(packet::FlowPacket *pFlowPacket);

        void SendToBond();
        void RecvFromConnection();
        void SendToConnection();

        std::string ToString();

        virtual ~Flow();

    protected:
        Flow(net::ipv4::SockAddr_In source, net::ipv4::SockAddr_In destination, net::ipv4::TcpConnection *tcpConnection, net::Bond *bond, context::Context *context);

    private:
        net::ipv4::TcpConnection *connection_;
        net::ipv4::SockAddr_In source_;
        net::ipv4::SockAddr_In destination_;
        net::Bond *bond_;
        context::Context *context_;

        collections::BlockingFlowPacketQueue toConnectionQueue_;

        collections::BlockingFlowPacketQueue toBondQueue_;
        std::atomic<uint64_t> toBondId_;

        std::atomic<bool> closed_;

        worker::Looper recvFromConnectionLooper_;
        worker::Looper sendToConnectionLooper_;
        worker::Looper sendToBondLooper_;
    }; // Flow

} // net

#endif //MULTIPATH_PROXY_FLOW_H
