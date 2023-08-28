//
// Created by Matthias Hofstätter on 27.08.23.
//

#include "PathBase.h"

#include "ContextBase.h"

namespace quic {
    PathBase::PathBase(ContextBase *context, uint64_t unique_path_id) : _context(context), _unique_path_id(unique_path_id) {

    }

    int PathBase::set_app_path_ctx(void *app_path_ctx) {
        return _context->set_app_path_ctx(_unique_path_id, app_path_ctx);
    }

    int PathBase::abandon_path(uint64_t reason, const char *phrase) {
        return _context->abandon_path(_unique_path_id, reason, phrase);
    }

    int PathBase::refresh_path_connection_id() {
        return _context->refresh_path_connection_id(_unique_path_id);
    }

    int PathBase::get_path_quality(picoquic_path_quality_t *quality) {
        return _context->get_path_quality(_unique_path_id, quality);
    }

    int PathBase::subscribe_to_quality_update_per_path(uint64_t pacing_rate_delta, uint64_t rtt_delta) {
        return _context->subscribe_to_quality_update_per_path(_unique_path_id, pacing_rate_delta, rtt_delta);
    }

    int PathBase::mark_datagram_ready_path(int is_path_ready) {
        return _context->mark_datagram_ready_path(_unique_path_id, is_path_ready);
    }

    int PathBase::stream_data_cb(picoquic_cnx_t *cnx, uint64_t stream_id, uint8_t *bytes, size_t length,
                                 picoquic_call_back_event_t fin_or_event, void *callback_ctx, void *v_stream_ctx) {
        return 0;
    }

    PathBase::~PathBase() {

    }
} // quic