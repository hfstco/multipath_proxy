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
        [[nodiscard]] uint64_t id() const;
        [[nodiscard]] bool finished() const;
        [[nodiscard]] bool reset() const;
        [[nodiscard]] uint64_t remoteStreamError() const;
        [[nodiscard]] Flow *flow() const;

        void MarkActiveStream();

        [[nodiscard]] std::string ToString() const;

        virtual ~QuicStream();

    protected:
        QuicStream(picoquic_cnx_t *quic_cnx, uint64_t id);

    private:
        picoquic_cnx_t *_quic_cnx;
        const uint64_t _id;
        std::atomic<bool> _active;
        std::atomic<bool> _finished;
        std::atomic<bool> _reset;
        std::atomic<uint64_t> _remoteStreamError;
        // TODO STREAM_DATA_BLOCKED, STOP_SENDING

        Flow *_flow = nullptr;
        std::atomic<bool> _flowHeaderSent = false;
        unsigned char *_rxBuffer = static_cast<unsigned char *>(malloc(1500));
        uint64_t _rxBufferSize = 0;
        backlog::Chunk *_chunkBuffer = nullptr;
        uint64_t _chunkBufferSize = 0;
    };

} // net

#endif //MPP_QUICSTREAM_H
