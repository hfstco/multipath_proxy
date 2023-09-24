//
// Created by Matthias Hofstätter on 01.08.23.
//

#include "Stream.h"

#include <cassert>

#include "Context.h"

namespace quic {
    Stream::Stream(Context *context, uint64_t stream_id) : StreamBase(context, stream_id) {
        VLOG(2) << "*" << to_string();
        LOG(INFO) << "New Stream with ID=" << _stream_id;
    };

    int Stream::callback(picoquic_cnx_t *cnx, uint64_t stream_id, uint8_t *bytes, size_t length,
                         picoquic_call_back_event_t fin_or_event, void *callback_ctx, void *v_stream_ctx) {
        assert(false);
        return 0;
    };

    std::string Stream::to_string() const {
        return "Stream[context=" + ((Context *)_context)->to_string() + ", stream_id=" + std::to_string(_stream_id) + "]";
    };

    Stream::~Stream() {
        VLOG(2) << "~" << to_string();
        LOG(INFO) << "Delete Stream with ID=" << _stream_id;
        //_context->delete_stream(_stream_id);
    }
} // quic