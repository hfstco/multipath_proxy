//
// Created by Matthias Hofstätter on 01.08.23.
//

#ifndef MPP_QUICSTREAM_H
#define MPP_QUICSTREAM_H

#include <cstdint>
#include <atomic>

#include "picoquic.h"

namespace backlog {
    class Chunk;
}

namespace net {

    class QuicConnection;
    class QuicClientConnection;
    class QuicServerConnection;
    class Flow;
    class Proxy;

    class QuicStream {
        friend class QuicConnection;
        friend class QuicClientConnection;
        friend class QuicServerConnection;
        friend class Proxy;
    public:
        [[nodiscard]] picoquic_cnx_t *quic_cnx() const;
        [[nodiscard]] uint64_t id() const;
        [[nodiscard]] Flow *flow() const;

        [[nodiscard]] bool active() const;
        [[nodiscard]] bool finished() const;
        [[nodiscard]] bool reset() const;

        [[nodiscard]] uint64_t remote_stream_error() const;

        [[nodiscard]] void mark_active_stream(int is_active = 1) const;
        void add_to_stream_with_ctx(const uint8_t * data, size_t length, int set_fin);

        [[nodiscard]] std::string to_string() const;

        virtual ~QuicStream();

    protected:
        picoquic_cnx_t *_quic_cnx;
        QuicConnection *_quic_connection;
        const uint64_t _stream_id;
        Flow *_flow;

        std::atomic<bool> _active;
        std::atomic<bool> _finished;
        std::atomic<bool> _reset;

        std::atomic<uint64_t> _remote_stream_error;

        unsigned char *_recv_buffer = static_cast<unsigned char *>(malloc(1500)); // TODO buffer size
        uint64_t _recv_buffer_size = 0;

        QuicStream(picoquic_cnx_t *quic_cnx, QuicConnection *quic_connection, uint64_t stream_id, Flow *flow);
    };

} // net

#endif //MPP_QUICSTREAM_H
