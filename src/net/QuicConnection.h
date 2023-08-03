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

    #define PICOQUIC_SAMPLE_ALPN "picoquic_sample"
    #define PICOQUIC_SAMPLE_SNI "test.example.com"

    #define PICOQUIC_NO_ERROR 0
    #define PICOQUIC_INTERNAL_ERROR 0x101
    #define PICOQUIC_NAME_TOO_LONG_ERROR 0x102
    #define PICOQUIC_NO_SUCH_FILE_ERROR 0x103
    #define PICOQUIC_FILE_READ_ERROR 0x104
    #define PICOQUIC_FILE_CANCEL_ERROR 0x105

    #define PICOQUIC_SERVER_QLOG_DIR ".";

    class QuicStream;

    class QuicConnection {
    public:
        QuicStream *CreateStream();
        QuicStream *CreateStream(uint64_t stream_id);
        void CloseStream(QuicStream *quic_stream);

        [[nodiscard]] bool disconnected() const;

        std::string ToString();

        virtual ~QuicConnection();

    protected:
        uint64_t _current_time = 0;
        picoquic_quic_t *_quic = nullptr;
        picoquic_cnx_t  *_quic_cnx = nullptr;
        std::future<int> _packet_loop;
        std::array<QuicStream *, 100> _streams{}; // TODO size
        std::atomic_flag _disconnected;

        QuicConnection();

        void disconnected(bool disconnected);
    };

} // net

#endif //MPP_QUICCONNECTION_H
