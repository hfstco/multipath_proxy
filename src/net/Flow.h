//
// Created by Matthias Hofstätter on 14.04.23.
//

#ifndef MULTIPATH_PROXY_FLOW_H
#define MULTIPATH_PROXY_FLOW_H

#include <atomic>
#include <thread>

#include "SockAddr.h"
#include "../worker/Looper.h"
#include "../backlog/SortedBacklog.h"
#include "../backlog/UnsortedBacklog.h"

namespace net {

    namespace ipv4 {
        class TcpConnection;
    }

    class Proxy;
    class QuicConnection;
    class QuicClientConnection;
    class QuicServerConnection;
    class QuicStream;

    class Flow {
        friend class Proxy;
        friend class QuicStream;
        friend class QuicClientConnection;
        friend class QuicServerConnection;
    public:
        static Flow *make(net::ipv4::SockAddr_In source, net::ipv4::SockAddr_In destination, net::ipv4::TcpConnection *tcp_connection);
        static Flow *make(net::ipv4::SockAddr_In source, net::ipv4::SockAddr_In destination, net::ipv4::TcpConnection *tcp_connection, uint64_t stream_id);

        net::ipv4::SockAddr_In source();
        net::ipv4::SockAddr_In destination();
        size_t size();
        bool use_satellite();

        backlog::UnsortedBacklog backlog;
        backlog::SortedBacklog &tx();

        void make_active(int is_active = 1);

        void recv_from_connection();
        void send_to_connection();

        std::string to_string();

        virtual ~Flow();

    protected:
        Flow(net::ipv4::SockAddr_In source, net::ipv4::SockAddr_In destination, net::ipv4::TcpConnection *tcp_connection);
        Flow(net::ipv4::SockAddr_In source, net::ipv4::SockAddr_In destination, net::ipv4::TcpConnection *tcp_connection, uint64_t stream_id);

    private:
        // tcp connection
        net::ipv4::SockAddr_In _source;
        net::ipv4::SockAddr_In _destination;
        net::ipv4::TcpConnection *_connection;

        // quic connection
        QuicStream *_ter_stream;
        QuicStream *_sat_stream;

        std::atomic<bool> _use_satellite;
        std::atomic<bool> _closed;

        std::atomic<uint64_t> _rx_offset;
        std::atomic<uint64_t> _sent_offset;

        backlog::SortedBacklog _tx_backlog;

        worker::Looper _recv_from_connection_looper;
        worker::Looper _send_to_connection_looper;
    }; // Flow

} // net

#endif //MULTIPATH_PROXY_FLOW_H
