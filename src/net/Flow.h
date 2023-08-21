//
// Created by Matthias Hofstätter on 14.04.23.
//

#ifndef MULTIPATH_PROXY_FLOW_H
#define MULTIPATH_PROXY_FLOW_H

#include <atomic>
#include <thread>

#include "SockAddr.h"
#include "../worker/Looper.h"
#include "../backlog/ChunkMap.h"
#include "../backlog/Backlog.h"

namespace net {

    namespace ipv4 {
        class TcpConnection;
    }

    class Proxy;
    class QuicStream;
    class QuicConnection;
    class QuicClientConnection;
    class QuicServerConnection;

    class Flow {
        friend class Proxy;
        friend class QuicStream;
        friend class QuicClientConnection;
        friend class QuicServerConnection;
    public:
        static Flow *make(net::ipv4::SockAddr_In source, net::ipv4::SockAddr_In destination, net::ipv4::TcpConnection *tcpConnection);

        net::ipv4::SockAddr_In source();
        net::ipv4::SockAddr_In destination();
        size_t size();
        bool useSatellite();

        backlog::Backlog &rx();
        backlog::Backlog &Backlog();
        backlog::ChunkMap &tx();

        void MakeActiveFlow(int isActive = 1);

        void RecvFromConnection();
        void SendToConnection();

        std::string ToString();

        virtual ~Flow();

    protected:
        Flow(net::ipv4::SockAddr_In source, net::ipv4::SockAddr_In destination, net::ipv4::TcpConnection *tcpConnection);

    private:
        net::ipv4::TcpConnection *_connection;
        net::ipv4::SockAddr_In _source;
        net::ipv4::SockAddr_In _destination;

        net::QuicStream *_terTxStream;
        net::QuicStream *_satTxStream;

        std::atomic<bool> _useSatellite;
        std::atomic<bool> _closed;

        std::atomic<uint64_t> _rxOffset;
        backlog::Backlog _rxBacklog;
        backlog::ChunkMap _txChunkMap;

        worker::Looper _recvFromConnectionLooper;
        worker::Looper _sendToConnectionLooper;
    }; // Flow

} // net

#endif //MULTIPATH_PROXY_FLOW_H
