//
// Created by Matthias Hofstätter on 27.08.23.
//

#include "StreamBase.h"

#include "ContextBase.h"

namespace quic {
    StreamBase::StreamBase(ContextBase *context, uint64_t stream_id) : _context(context), _stream_id(stream_id) {

    }

    int StreamBase::set_stream_path_affinity(uint64_t unique_path_id) {
        return _context->set_stream_path_affinity(_stream_id, unique_path_id);
    }

    int StreamBase::mark_direct_receive_stream(picoquic_stream_direct_receive_fn direct_receive_fn,
                                               void *direct_receive_ctx) {
        return _context->mark_direct_receive_stream(_stream_id, direct_receive_fn, direct_receive_ctx);
    }

    int StreamBase::set_app_stream_ctx(void *app_stream_ctx) {
        return _context->set_app_stream_ctx(_stream_id, app_stream_ctx);
    }

    void StreamBase::unlink_app_stream_ctx() {
        _context->unlink_app_stream_ctx(_stream_id);
    }

    int StreamBase::mark_active_stream(int is_active, void *v_stream_ctx) {
        return _context->mark_active_stream(_stream_id, is_active, v_stream_ctx);
    }

    int StreamBase::set_stream_priority(uint8_t stream_priority) {
        return _context->set_stream_priority(_stream_id, stream_priority);
    }

    int StreamBase::mark_high_priority_stream(int is_high_priority) {
        return _context->mark_high_priority_stream(_stream_id, is_high_priority);
    }

    int StreamBase::add_to_stream(const uint8_t *data, size_t length, int set_fin) {
        return _context->add_to_stream(_stream_id, data, length, set_fin);
    }

    void StreamBase::reset_stream_ctx() {
        _context->reset_stream_ctx(_stream_id);
    }

    int StreamBase::add_to_stream_with_ctx(const uint8_t *data, size_t length, int set_fin, void *app_stream_ctx) {
        return _context->add_to_stream_with_ctx(_stream_id, data, length, set_fin, app_stream_ctx);
    }

    int StreamBase::reset_stream(uint64_t local_stream_error) {
        return _context->reset_stream(_stream_id, local_stream_error);
    }

    int StreamBase::open_flow_control(uint64_t expected_data_size) {
        return _context->open_flow_control(_stream_id, expected_data_size);
    }

    int StreamBase::stop_sending(uint16_t local_stream_error) {
        return _context->stop_sending(_stream_id, local_stream_error);
    }

    int StreamBase::discard_stream(uint16_t local_stream_error) {
        return _context->discard_stream(_stream_id, local_stream_error);
    }

    uint64_t StreamBase::get_remote_stream_error() {
        return _context->get_remote_stream_error(_stream_id);
    }

    int StreamBase::stream_data_cb(picoquic_cnx_t *cnx, uint64_t stream_id, uint8_t *bytes, size_t length,
                                   picoquic_call_back_event_t fin_or_event, void *callback_ctx, void *v_stream_ctx) {
        return 0;
    }

    int StreamBase::stream_direct_receive(picoquic_cnx_t *cnx, uint64_t stream_id, int fin, const uint8_t *bytes,
                                          uint64_t offset, size_t length, void *direct_receive_ctx) {
        return 0;
    }

    StreamBase::~StreamBase() {

    }
} // quic