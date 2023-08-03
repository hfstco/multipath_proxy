//
// Created by Matthias Hofstätter on 01.08.23.
//

#ifndef MPP_QUICSTREAM_H
#define MPP_QUICSTREAM_H

#include <cstdint>
#include <atomic>

#include "picoquic.h"

namespace net {

    class QuicConnection;
    class QuicClientConnection;
    class QuicServerConnection;

    class QuicStream {
        friend class QuicConnection;
        friend class QuicClientConnection;
        friend class QuicServerConnection;
    public:
        [[nodiscard]] const uint64_t id() const;

        [[nodiscard]] const std::atomic_flag &reset() const;

        [[nodiscard]] const std::atomic_flag &finished() const;

        [[nodiscard]] const std::atomic<uint64_t> &recvBytes() const;

        [[nodiscard]] const std::atomic<uint64_t> &sendBytes() const;

        int Send(unsigned char *data, size_t size);
        int Recv(unsigned char *data, size_t size);

        std::string ToString() const;

        virtual ~QuicStream();

    protected:
        QuicStream(QuicConnection *quic_connection, uint64_t id);

        void remoteError(uint64_t remote_error);
        void reset(bool reset);
        void finished(bool finished);

    private:
        const uint64_t _id;
        QuicConnection *_quic_connection;

        uint8_t *_rx_data = {};
        size_t _rx_size;
        uint8_t *_tx_data = {};
        size_t _tx_size;

        std::atomic_flag _reset;
        std::atomic_flag _finished;
        std::atomic<uint64_t> _remote_error;
        std::atomic<uint64_t> _recv_bytes;
        std::atomic<uint64_t> _send_bytes;
    };

} // net

#endif //MPP_QUICSTREAM_H
