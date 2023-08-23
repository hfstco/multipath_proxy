//
// Created by Matthias Hofstätter on 31.07.23.
//

#ifndef MPP_QUICCONNECTION_H
#define MPP_QUICCONNECTION_H

#include <cstdint>
#include <future>
#include <array>

#include <picoquic.h>
#include <picosocks.h>
#include <picoquic_utils.h>
#include <autoqlog.h>

namespace net {

    #define PICOQUIC_SAMPLE_ALPN "mpp"

    class QuicServerConnection;
    class QuicClientConnection;
    class QuicStream;
    class Flow;

    class QuicConnection {
        // static context
        friend class QuicServerConnection;
        friend class QuicClientConnection;
    public:
        [[nodiscard]] picoquic_cnx_t *quic_cnx() const;
        [[nodiscard]] bool disconnected() const;
        [[nodiscard]] uint64_t local_error() const;
        [[nodiscard]] uint64_t remote_error() const;
        [[nodiscard]] uint64_t application_error() const;

        // streams
        // https://datatracker.ietf.org/doc/html/rfc9000#name-stream-types-and-identifier
        QuicStream *create_stream(net::Flow *flow);
        QuicStream *create_stream(uint64_t stream_id, net::Flow *flow);
        bool stream_exists(uint64_t stream_id);
        void close_stream(uint64_t stream_id);
        void mark_active_stream(uint64_t stream_id, int is_active, void *v_stream_ctx);
        void set_app_stream_ctx(uint64_t stream_id, void *app_stream_ctx);
        void add_to_stream_with_ctx(uint64_t stream_id, const uint8_t * data, size_t length, int set_fin, void * app_stream_ctx);


        virtual std::string to_string();

        virtual ~QuicConnection();

    protected:
        uint64_t _current_time = 0;
        picoquic_quic_t *_quic = nullptr;
        picoquic_cnx_t  *_quic_cnx = nullptr;
        std::future<int> _packet_loop;
        static std::recursive_mutex _quic_mutex;

        std::array<QuicStream *, 10000> _streams = { nullptr }; // TODO max array size in cpp = uint32_t::max

        std::atomic<bool> _disconnected;
        uint64_t _local_error = 0;
        uint64_t _remote_error = 0;
        uint64_t _application_error = 0;

        const bool _is_sat;

        explicit QuicConnection(bool is_sat);
    };

} // net

#endif //MPP_QUICCONNECTION_H
