//
// Created by Matthias Hofstätter on 31.07.23.
//

#include "QuicConnection.h"

#include <glog/logging.h>

#include "QuicStream.h"
#include "Flow.h"

namespace net {
    QuicConnection::QuicConnection(bool is_sat) : _current_time(picoquic_current_time()), _disconnected(false), _is_sat(is_sat) {
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

    uint64_t QuicConnection::localError() const {
        return _localError;
    }

    uint64_t QuicConnection::remoteError() const {
        return _remoteError;
    }

    uint64_t QuicConnection::applicationError() const {
        return _applicationError;
    }

    QuicStream *QuicConnection::ActivateStream(uint64_t streamId) {
        std::lock_guard lock(_streams_mutex);

        LOG(ERROR) << ToString() << ".ActivateStream(" << streamId << ")";

        if (_streams[streamId] != nullptr) {
            LOG(ERROR) << "Stream exists";
            return nullptr;
        }

        auto *quicStream = new QuicStream(_quic_cnx, streamId);
        _streams[streamId] = quicStream;
        return quicStream;
    }

    bool QuicConnection::StreamExists(uint64_t streamId) {
        return _streams[streamId] != nullptr;
    }

    void QuicConnection::CloseStream(uint64_t streamId) {
        std::lock_guard lock(_streams_mutex);

        LOG(INFO) << ToString() << ".CloseStream(streamId=" << streamId << ")";

        delete _streams[streamId];
        _streams[streamId] = nullptr;
    }

    std::string QuicConnection::ToString() {
        return "QuicConnection[quic=[skip]]"; // TODO quic
    }

    QuicConnection::~QuicConnection() {
        LOG(ERROR) << ToString() << ".~QuicConnection()";

        picoquic_set_callback(_quic_cnx, nullptr, nullptr);

        for(auto & _stream : _streams)
        {
            if (_stream != nullptr) {
                delete _stream;
                _stream = nullptr;
            }
        }

        if(_quic_cnx != nullptr) {
            picoquic_close(_quic_cnx, 0);
        }

        if (_quic != nullptr) {
            picoquic_free(_quic);
        }
    }
} // net