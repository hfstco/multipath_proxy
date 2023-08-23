//
// Created by Matthias Hofstätter on 01.08.23.
//

#include "QuicStream.h"

#include <glog/logging.h>
#include <cassert>

#include "QuicConnection.h"
#include "../packet/FlowHeader.h"
#include "Flow.h"

namespace net {
    QuicStream::QuicStream(picoquic_cnx_t *quic_cnx, QuicConnection *quic_connection, uint64_t stream_id, Flow *flow) :     _quic_cnx(quic_cnx),
                                                                                                                            _quic_connection(quic_connection),
                                                                                                                           _stream_id(stream_id),
                                                                                                                           _flow(flow),
                                                                                                                           _active(false),
                                                                                                                           _reset(false),
                                                                                                                           _finished(false),
                                                                                                                           _remote_stream_error(0) {
        LOG(INFO) << "QuicStream::QuicStream(stream_id=" << stream_id << ")";

        // set app ctx
        _quic_connection->set_app_stream_ctx(_stream_id, this);

        // only if steam type == connection type
        if (picoquic_is_client(_quic_cnx) == PICOQUIC_IS_CLIENT_STREAM_ID(stream_id)) {
            // send flow header
            packet::FlowHeader flowHeader = {
                    .source_ip = flow->source().sin_addr,
                    .destination_ip = flow->destination().sin_addr,
                    .source_port = flow->source().sin_port,
                    .destination_port = flow->destination().sin_port
            };

            add_to_stream_with_ctx(reinterpret_cast<const uint8_t *>(&flowHeader), sizeof(packet::FlowHeader), 0);

            // mark active
            mark_active_stream(1);
        }
    }

    picoquic_cnx_t *QuicStream::quic_cnx() const {
        return _quic_cnx;
    }

    uint64_t QuicStream::id() const {
        return _stream_id;
    }

    bool QuicStream::finished() const {
        return _finished;
    }

    bool QuicStream::reset() const {
        return _reset;
    }

    uint64_t QuicStream::remote_stream_error() const {
        return _remote_stream_error;
    }

    std::string QuicStream::to_string() const {
        std::stringstream ss;
        ss << std::boolalpha << "QuicStream[id=" << std::to_string(_stream_id) << ", finished=" << _finished << ", reset=" << _reset << ", remote_stream_error=" << std::to_string(_remote_stream_error) << "]";
        return ss.str();
    }

    QuicStream::~QuicStream() {
        LOG(ERROR) << to_string() << ".~QuicStream()";
    }

    bool QuicStream::active() const {
        return _active;
    }

    Flow *QuicStream::flow() const {
        return _flow;
    }

    void QuicStream::mark_active_stream(int is_active) const {
        LOG(INFO) << to_string() << ".mark_active(is_active=" << is_active << ")";

        _quic_connection->mark_active_stream(_stream_id, is_active, (void *) this);
    }

    void QuicStream::add_to_stream_with_ctx(const uint8_t *data, size_t length, int set_fin) {
        LOG(INFO) << to_string() << ".add_to_stream_with_ctx(data=SKIP, length=" << length << ", set_fin=" << set_fin << ")";

        _quic_connection->add_to_stream_with_ctx(_stream_id, data, length, set_fin, this);
    }

} // net