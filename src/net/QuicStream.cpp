//
// Created by Matthias Hofstätter on 01.08.23.
//

#include "QuicStream.h"

#include <glog/logging.h>

#include "QuicConnection.h"

namespace net {
    QuicStream::QuicStream(QuicConnection *quic_connection, uint64_t id) : _quic_connection(quic_connection), _id(id), _finished(false), _reset(false) {
        LOG(ERROR) << "QuicStream::QuicStream(quic_connection=" << quic_connection << ", id=" << id << ")";
    }

    const uint64_t QuicStream::id() const {
        return _id;
    }

    const std::atomic_flag &QuicStream::reset() const {
        return _reset;
    }

    const std::atomic_flag &QuicStream::finished() const {
        return _finished;
    }

    const std::atomic<uint64_t> &QuicStream::recvBytes() const {
        return _recv_bytes;
    }

    const std::atomic<uint64_t> &QuicStream::sendBytes() const {
        return _send_bytes;
    }

    void QuicStream::remoteError(uint64_t remote_error) {
        _remote_error.store(remote_error);
    }

    void QuicStream::reset(bool reset) {
        if (reset)
            _reset.test_and_set();
        else
            _reset.clear();
    }

    void QuicStream::finished(bool finished) {
        if(finished)
            _finished.test_and_set();
        else
            _finished.clear();
    }

    int QuicStream::Send(unsigned char *data, size_t size) {
        LOG(ERROR) << ToString() << ".send()"; // TODO params

        // TODO

        return 0;
    }

    int QuicStream::Recv(unsigned char *data, size_t size) {
        LOG(ERROR) << ToString() << ".recv()"; // TODO params

        // TODO

        return 0;
    }

    std::string QuicStream::ToString() const {
        return "QuicStream[id=" + std::to_string(_id) + "]";
    }

    QuicStream::~QuicStream() {
        LOG(ERROR) << ToString() << ".~QuicStream()";
    }
} // net