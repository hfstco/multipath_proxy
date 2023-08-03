//
// Created by Matthias Hofstätter on 31.07.23.
//

#include "QuicConnection.h"

#include <glog/logging.h>

#include "QuicStream.h"

namespace net {
    QuicConnection::QuicConnection() : _current_time(picoquic_current_time()), _disconnected(false) {
        LOG(ERROR) << "QuicConnection::QuicConnection()";
    }

    bool QuicConnection::disconnected() const {
        return _disconnected.test();
    }

    void QuicConnection::disconnected(bool disconnected) {
        if(disconnected) {
            _disconnected.test_and_set();
        } else {
            _disconnected.clear();
        }
    }

    QuicStream *QuicConnection::CreateStream(uint64_t stream_id) {
        LOG(ERROR) << ToString() << ".CreateStream(" << stream_id << ")";

        if (_streams[stream_id] != nullptr) {
            LOG(ERROR) << "Stream exists";
            return nullptr;
        }

        auto *quicStream = new QuicStream(this, stream_id);
        _streams[stream_id] = quicStream;
        return quicStream;
    }

    QuicStream *QuicConnection::CreateStream() {
        LOG(INFO) << ToString() << ".CreateStream()";

        uint64_t id = -1;
        for (uint64_t i = 0; i < _streams.size(); i++) {
            if (_streams[i] == nullptr) {
                id = i;
            }
        }

        if (id == -1) {
            LOG(ERROR) << "No free streams.";
            return nullptr;
        }

        auto *quicStream = new QuicStream(this, id);
        _streams[id] = quicStream;
        return quicStream;
    }

    void QuicConnection::CloseStream(QuicStream *quicStream) {
        LOG(INFO) << ToString() << ".CloseStream(quicStream->id()=" << quicStream->id() << ")";

        _streams[quicStream->id()] = nullptr;

        delete quicStream;
    }

    std::string QuicConnection::ToString() {
        return "QuicConnection[quic=[skip], streams=" + std::to_string(_streams.size()) + "]"; // TODO quic
    }

    QuicConnection::~QuicConnection() {
        LOG(ERROR) << ToString() << ".~QuicConnection()";

        //int ret = _packet_loop.get();

        if (_quic != nullptr) {
            picoquic_free(_quic);
        }

        for(auto & _stream : _streams)
        {
            if (_stream != nullptr) {
                delete _stream;
                _stream = nullptr;
            }
        }
    }
} // net