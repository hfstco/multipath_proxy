//
// Created by Matthias Hofstätter on 14.04.23.
//

#ifndef MULTIPATH_PROXY_FLOW_H
#define MULTIPATH_PROXY_FLOW_H

#include <atomic>
#include <thread>

#include "../net/SockAddr.h"
#include "../worker/Looper.h"
#include "../backlog/UnsortedBacklog.h"
#include "../quic/Stream.h"
#include "Buffer.h"

namespace net::ipv4 {
    class TcpConnection;
}

namespace backlog {
    struct Chunk;
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

        [[nodiscard]] std::string to_string() const {
            return "FlowHeader[source=" + net::ipv4::SockAddr_In(source_ip, source_port).to_string() + ", destination=" +
                   net::ipv4::SockAddr_In(destination_ip, destination_port).to_string() + "]";
        };
    };

    class Flow : public quic::Stream {
        friend class quic::FlowContext;
        friend class quic::Context;
    public:
        backlog::UnsortedBacklog backlog;
        std::atomic<std::chrono::time_point<std::chrono::steady_clock>> last_satellite_timestamp;

        void recv_from_connection();

        std::string to_string();

        int prepare_to_send(uint8_t *bytes, size_t length);

        int stream_data(uint8_t *bytes, size_t length);

        int stream_fin(uint8_t *bytes, size_t length);

        virtual ~Flow();

    private:
        net::ipv4::SockAddr_In _source;
        net::ipv4::SockAddr_In _destination;
        // tcp connection
        net::ipv4::TcpConnection *_connection;

        std::mutex _recving;
        std::atomic<bool> _active = false;
        std::atomic<uint64_t> _path = 0;

        backlog::Chunk *_tx_buffer;
        uint64_t _tx_buffer_offset;

        worker::Looper _recv_from_connection_looper;

        Flow(quic::FlowContext *context, uint64_t stream_id, net::ipv4::SockAddr_In source, net::ipv4::SockAddr_In destination, net::ipv4::TcpConnection *tcp_connection);
    };

} // flow

#endif //MULTIPATH_PROXY_FLOW_H
