//
// Created by Matthias Hofstätter on 14.04.23.
//

#ifndef MULTIPATH_PROXY_FLOW_H
#define MULTIPATH_PROXY_FLOW_H

#include <atomic>
#include <thread>

#include "../net/SockAddr.h"
#include "../worker/Looper.h"
#include "../backlog/SortedBacklog.h"
#include "../backlog/UnsortedBacklog.h"
#include "../quic/Stream.h"
#include "Buffer.h"

namespace net::ipv4 {
    class TcpConnection;
}

namespace quic {
    class FlowContext;
}

namespace flow {

    struct __attribute__((packed)) FlowHeader {
        struct in_addr source_ip;           // 4
        struct in_addr destination_ip;      // 8
        in_port_t source_port;              // 10
        in_port_t destination_port;         // 12

        std::string to_string() const {
            return "FlowHeader[source=" + net::ipv4::SockAddr_In(source_ip, source_port).to_string() + ", destination=" +
                   net::ipv4::SockAddr_In(destination_ip, destination_port).to_string() + "]";
        };
    };

    class Flow : public quic::Stream {
        friend class quic::FlowContext;
        friend class quic::Context;
    public:
        net::ipv4::SockAddr_In source();
        net::ipv4::SockAddr_In destination();

        backlog::SortedBacklog &tx();

        void recv_from_connection();
        void send_to_connection();

        int callback(picoquic_cnx_t *cnx, uint64_t stream_id, uint8_t *bytes, size_t length,
                     picoquic_call_back_event_t fin_or_event, void *callback_ctx, void *v_stream_ctx) override;

        std::string to_string();

        virtual ~Flow();

    private:
        quic::FlowContext *_context;
        // tcp connection
        net::ipv4::SockAddr_In _source;
        net::ipv4::SockAddr_In _destination;
        net::ipv4::TcpConnection *_connection;

        std::atomic<bool> _use_satellite;
        std::atomic<bool> _closed;

        std::atomic<uint64_t> _rx_offset;
        backlog::UnsortedBacklog backlog;
        backlog::SortedBacklog _tx_backlog;

        // TODO TODO rx buffer!
        flow::Buffer<1523> _rx_buffer;

        worker::Looper _recv_from_connection_looper;
        worker::Looper _send_to_connection_looper;

        Flow(quic::FlowContext *context, uint64_t stream_id, net::ipv4::SockAddr_In source, net::ipv4::SockAddr_In destination, net::ipv4::TcpConnection *tcp_connection);
    };

} // flow

#endif //MULTIPATH_PROXY_FLOW_H
