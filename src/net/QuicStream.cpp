//
// Created by Matthias Hofstätter on 01.08.23.
//

#include "QuicStream.h"

#include <glog/logging.h>

#include "QuicConnection.h"
#include "Flow.h"

namespace net {
    QuicStream::QuicStream(picoquic_cnx_t *quic_cnx, uint64_t id, net::Flow *flow) : _quic_cnx(quic_cnx),
                                                                                     _id(id),
                                                                                     _flow(flow),
                                                                                     _active(false),
                                                                                     _reset(false),
                                                                                     _finished(false),
                                                                                     _remoteStreamError(0) {
        LOG(INFO) << "QuicStream::QuicStream(quic_cnx=" << quic_cnx << ", id=" << id << ")";

        if (picoquic_set_app_stream_ctx(_quic_cnx, _id, this) != 0) {
            /* Internal error */
            (void) picoquic_reset_stream(_quic_cnx, _id, PICOQUIC_INTERNAL_ERROR);

            //throw std::exception();
        }
    }

    uint64_t QuicStream::id() const {
        return _id;
    }

    bool QuicStream::finished() const {
        return _finished.load();
    }

    bool QuicStream::reset() const {
        return _reset.load();
    }

    uint64_t QuicStream::remoteStreamError() const {
        return _remoteStreamError.load();
    }

    Flow *QuicStream::flow() const {
        return _flow;
    }

    std::string QuicStream::ToString() const {
        std::stringstream ss;
        ss << std::boolalpha << "QuicStream[quic_cnx=" << _quic_cnx << ", id=" << std::to_string(_id) << ", finished=" << _finished.load() << ", reset=" << _reset.load() << ", remoteStreamError=" << std::to_string(_remoteStreamError) << "]";
        return ss.str();
    }

    QuicStream::~QuicStream() {
        LOG(ERROR) << ToString() << ".~QuicStream()";

        picoquic_reset_stream(_quic_cnx, _id, 0);
    }

    void QuicStream::MarkActiveStream(int isActive) {
        LOG(INFO) << ToString() << ".MarkActiveStream(isActive=" << isActive << ")";

        int ret = picoquic_mark_active_stream(_quic_cnx, _id, isActive, this);

        if (ret != 0) {
            /* Internal error */
            (void) picoquic_reset_stream(_quic_cnx, _id, PICOQUIC_INTERNAL_ERROR);

            //throw Exception("picoquic_mark_active_stream");
        }

        _active.store(true);
    }

    void QuicStream::ResetStream(uint64_t local_stream_error) {
        LOG(INFO) << ToString() << ".ResetStream(local_stream_error=" << local_stream_error << ")";

        picoquic_reset_stream(_quic_cnx, _id, 0);
    }

} // net