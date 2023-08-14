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

    #define PICOQUIC_NO_ERROR 0
    #define PICOQUIC_INTERNAL_ERROR 0x101
    #define PICOQUIC_NAME_TOO_LONG_ERROR 0x102
    #define PICOQUIC_NO_SUCH_FILE_ERROR 0x103
    #define PICOQUIC_FILE_READ_ERROR 0x104
    #define PICOQUIC_FILE_CANCEL_ERROR 0x105

    class QuicStream;
    class Flow;

    class QuicConnection {
        friend class QuicClientConnection;
        friend class QuicServerConnection;
    public:
        // https://datatracker.ietf.org/doc/html/rfc9000#name-stream-types-and-identifier
        virtual QuicStream *CreateStream(bool unidirectional) = 0;
        QuicStream *ActivateStream(uint64_t streamId);
        bool StreamExists(uint64_t streamId);
        void CloseStream(uint64_t streamId);

        [[nodiscard]] bool disconnected() const;
        [[nodiscard]] uint64_t localError() const;
        [[nodiscard]] uint64_t remoteError() const;
        [[nodiscard]] uint64_t applicationError() const;

        virtual std::string ToString();

        virtual ~QuicConnection();

    protected:
        uint64_t _current_time = 0;
        picoquic_quic_t *_quic = nullptr;
        picoquic_cnx_t  *_quic_cnx = nullptr;
        std::future<int> _packet_loop;
        std::array<QuicStream *, 100> _streams = { nullptr }; // TODO size
        std::recursive_mutex _streams_mutex;

        std::atomic_flag _disconnected;
        uint64_t _localError = 0;
        uint64_t _remoteError = 0;
        uint64_t _applicationError = 0;

        const bool _is_sat;

        explicit QuicConnection(bool is_sat);

        void disconnected(bool disconnected);
    };

} // net

#endif //MPP_QUICCONNECTION_H
