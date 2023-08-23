//
// Created by Matthias Hofstätter on 31.07.23.
//

#include "QuicConnection.h"

#include <glog/logging.h>
#include <cassert>

#include "QuicStream.h"
#include "Flow.h"

namespace net {
    std::recursive_mutex QuicConnection::_quic_mutex;

    QuicConnection::QuicConnection(bool is_sat) : _current_time(picoquic_current_time()), _disconnected(true), _is_sat(is_sat) {
        LOG(ERROR) << "QuicConnection::QuicConnection()";
    }

    picoquic_cnx_t *QuicConnection::quic_cnx() const {
        return _quic_cnx;
    }

    bool QuicConnection::disconnected() const {
        return _disconnected;
    }

    uint64_t QuicConnection::local_error() const {
        return _local_error;
    }

    uint64_t QuicConnection::remote_error() const {
        return _remote_error;
    }

    uint64_t QuicConnection::application_error() const {
        return _application_error;
    }

    QuicStream *QuicConnection::create_stream(net::Flow *flow) {
        std::lock_guard lock(_quic_mutex);

        return create_stream(picoquic_get_next_local_stream_id(_quic_cnx, 1), flow);
    }

    QuicStream *QuicConnection::create_stream(uint64_t stream_id, net::Flow *flow) {
        std::lock_guard lock(_quic_mutex);

        LOG(ERROR) << to_string() << ".create_stream(stream_id=" << stream_id << ", _flow=" << flow->to_string() << ")";

        assert(_streams[stream_id] == nullptr);

        if (_streams[stream_id] != nullptr) {
            LOG(ERROR) << "Stream exists";
            return nullptr;
        }

        _streams[stream_id] = new QuicStream(_quic_cnx, this, stream_id, flow);
        return _streams[stream_id];
    }

    bool QuicConnection::stream_exists(uint64_t stream_id) {
        std::lock_guard lock(_quic_mutex);

        return _streams[stream_id] != nullptr;
    }

    void QuicConnection::close_stream(uint64_t stream_id) {
        std::lock_guard lock(_quic_mutex);

        LOG(INFO) << to_string() << ".close_stream(stream_id=" << stream_id << ")";

        assert(_streams[stream_id] != nullptr);

        if (_streams[stream_id] != nullptr) {
            delete _streams[stream_id];
            _streams[stream_id] = nullptr;
        }
    }

    std::string QuicConnection::to_string() {
        return "QuicConnection[is_sat=" + std::to_string(_is_sat) + "]"; // TODO quic
    }

    QuicConnection::~QuicConnection() {
        std::lock_guard lock(_quic_mutex);

        LOG(ERROR) << to_string() << ".~QuicConnection()";

        picoquic_set_callback(_quic_cnx, nullptr, nullptr);

        for(auto &stream : _streams)
        {
            if (stream != nullptr) {
                delete stream;
                stream = nullptr;
            }
        }

        if (_quic != nullptr) {
            picoquic_free(_quic);
        }

        if(_quic_cnx != nullptr) {
            picoquic_close(_quic_cnx, 0);
        }
    }

    void QuicConnection::mark_active_stream(uint64_t stream_id, int is_active, void *v_stream_ctx) {
        std::lock_guard lock(_quic_mutex);

        if (picoquic_mark_active_stream(_quic_cnx, stream_id, is_active, v_stream_ctx) != 0) {
            // TODO
            assert(false);
        }
    }

    void QuicConnection::set_app_stream_ctx(uint64_t stream_id, void *app_stream_ctx) {
        std::lock_guard lock(_quic_mutex);

        if (picoquic_set_app_stream_ctx(_quic_cnx, stream_id, app_stream_ctx) != 0) {
            // TODO
            assert(false);
        }
    }

    void QuicConnection::add_to_stream_with_ctx(uint64_t stream_id, const uint8_t *data, size_t length, int set_fin,
                                                void *app_stream_ctx) {
        std::lock_guard lock(_quic_mutex);

        if (picoquic_add_to_stream_with_ctx(_quic_cnx, stream_id, data, length, set_fin, app_stream_ctx) != 0) {
            // TODO
            assert(false);
        }
    }
} // net