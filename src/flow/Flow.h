//
// Created by Matthias Hofstätter on 14.04.23.
//

#ifndef MULTIPATH_PROXY_FLOW_H
#define MULTIPATH_PROXY_FLOW_H

#include <atomic>
#include <thread>

#include "../net/SockAddr.h"
#include "../worker/Looper.h"
#include "../backlog/ChunkQueue.h"
#include "../quic/Stream.h"
#include "../net/TcpSocket.h"
#include "../backlog/Backlog.h"

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
        struct in_addr ip;      // 8
        in_port_t port;         // 12

        [[nodiscard]] std::string to_string() const {
            return "FlowHeader[source=" + net::ipv4::SockAddr_In(ip, port).to_string() + "]";
        };
    };

    class Flow : public quic::Stream {
        // new_flow()
        friend class quic::FlowContext;
        // new_stream()
        friend class quic::Context;
    public:
        // tcp connection
        void recv_from_connection();
        void send_to_connection();

        // quic connection
        int prepare_to_send(uint8_t *bytes, size_t length);
        int stream_data(uint8_t *bytes, size_t length);
        int stream_fin(uint8_t *bytes, size_t length);

        std::string to_string();

        virtual ~Flow();

    private:
        // tcp connection
        net::ipv4::TcpConnection *_connection;

        // rx pipe
        std::mutex _rx_mutex;
        int _rx_pipe[2];
        std::atomic<size_t> _backlog;

        // tx pipe
        //std::mutex _tx_mutex;
        int _tx_pipe[2];
        std::atomic<size_t> _tx_size;

        std::atomic<bool> _active;
        std::atomic<bool> _closed;
        std::atomic<uint64_t> _path;

        std::atomic<std::chrono::time_point<std::chrono::steady_clock>> _last_satellite_timestamp;

        Flow(quic::FlowContext *context, uint64_t stream_id, net::ipv4::TcpConnection *connection);
    };

} // flow

#endif //MULTIPATH_PROXY_FLOW_H
