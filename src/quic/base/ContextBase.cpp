//
// Created by Matthias Hofstätter on 27.08.23.
//

#include "ContextBase.h"

#include <picoquic.h>
#include <tls_api.h>
#include <cassert>

#include "../Context.h"

namespace quic {
    ContextBase::ContextBase(QuicBase *quic, picoquic_cnx_t *cnx) : _quic(quic), _cnx(cnx) {

    }

    void ContextBase::log_app_message_v(const char *fmt, va_list vargs) {
        std::lock_guard lock(_mutex);

        picoquic_log_app_message_v(_cnx, fmt, vargs);
    }

    void ContextBase::log_app_message(const char *fmt, ...) {
        std::lock_guard lock(_mutex);

        assert(false);
        // TODO ...
        picoquic_log_app_message(_cnx, fmt);
    }

    void
    ContextBase::get_close_reasons(uint64_t *local_reason, uint64_t *remote_reason, uint64_t *local_application_reason,
                                   uint64_t *remote_application_reason) {
        std::lock_guard lock(_mutex);

        picoquic_get_close_reasons(_cnx, local_reason, remote_reason, local_application_reason, remote_application_reason);
    }

    void ContextBase::set_transport_parameters(const picoquic_tp_t *tp) {
        std::lock_guard lock(_mutex);

        picoquic_set_transport_parameters(_cnx, tp);
    }

    picoquic_tp_t const *ContextBase::get_transport_parameters(int get_local) {
        std::lock_guard lock(_mutex);

        return picoquic_get_transport_parameters(_cnx, get_local);
    }

    int ContextBase::start_client_cnx() {
        std::lock_guard lock(_mutex);

        return picoquic_start_client_cnx(_cnx);
    }

    int ContextBase::close(uint16_t application_reason_code) {
        std::lock_guard lock(_mutex);

        return picoquic_close(_cnx, application_reason_code);
    }

    void ContextBase::close_immediate() {
        std::lock_guard lock(_mutex);

        picoquic_close_immediate(_cnx);
    }

    void ContextBase::delete_cnx() {
        std::lock_guard lock(_mutex);

        picoquic_delete_cnx(_cnx);
    }

    void ContextBase::set_desired_version(uint32_t desired_version) {
        std::lock_guard lock(_mutex);

        picoquic_set_desired_version(_cnx, desired_version);
    }

    void ContextBase::set_rejected_version(uint32_t rejected_version) {
        std::lock_guard lock(_mutex);

        picoquic_set_rejected_version(_cnx, rejected_version);
    }

    int ContextBase::probe_new_path(const struct sockaddr *addr_from, const struct sockaddr *addr_to,
                                    uint64_t current_time) {
        std::lock_guard lock(_mutex);

        return picoquic_probe_new_path(_cnx, addr_from, addr_to, current_time);
    }

    int ContextBase::probe_new_path_ex(const struct sockaddr *addr_from, const struct sockaddr *addr_to, int if_index,
                                       uint64_t current_time, int to_preferred_address) {
        std::lock_guard lock(_mutex);

        return picoquic_probe_new_path_ex(_cnx, addr_from, addr_to, if_index, current_time, to_preferred_address);
    }

    void ContextBase::enable_path_callbacks(int are_enabled) {
        std::lock_guard lock(_mutex);

        picoquic_enable_path_callbacks(_cnx, are_enabled);
    }

    int ContextBase::set_app_path_ctx(uint64_t unique_path_id, void *app_path_ctx) {
        std::lock_guard lock(_mutex);

        return picoquic_set_app_path_ctx(_cnx, unique_path_id, app_path_ctx);
    }

    int ContextBase::abandon_path(uint64_t unique_path_id, uint64_t reason, const char *phrase) {
        std::lock_guard lock(_mutex);

        return picoquic_abandon_path(_cnx, unique_path_id, reason, phrase);
    }

    int ContextBase::refresh_path_connection_id(uint64_t unique_path_id) {
        std::lock_guard lock(_mutex);

        return picoquic_refresh_path_connection_id(_cnx, unique_path_id);
    }

    int ContextBase::set_stream_path_affinity(uint64_t stream_id, uint64_t unique_path_id) {
        std::lock_guard lock(_mutex);

        return picoquic_set_stream_path_affinity(_cnx, stream_id, unique_path_id);
    }

    int ContextBase::get_path_quality(uint64_t unique_path_id, picoquic_path_quality_t *quality) {
        std::lock_guard lock(_mutex);

        return picoquic_get_path_quality(_cnx, unique_path_id, quality);
    }

    void ContextBase::get_default_path_quality(picoquic_path_quality_t *quality) {
        std::lock_guard lock(_mutex);

        picoquic_get_default_path_quality(_cnx, quality);
    }

    int ContextBase::subscribe_to_quality_update_per_path(uint64_t unique_path_id, uint64_t pacing_rate_delta,
                                                          uint64_t rtt_delta) {
        std::lock_guard lock(_mutex);

        return picoquic_subscribe_to_quality_update_per_path(_cnx, unique_path_id, pacing_rate_delta, rtt_delta);
    }

    void ContextBase::subscribe_to_quality_update(uint64_t pacing_rate_delta, uint64_t rtt_delta) {
        std::lock_guard lock(_mutex);

        picoquic_subscribe_to_quality_update(_cnx, pacing_rate_delta, rtt_delta);
    }

    int ContextBase::start_key_rotation() {
        std::lock_guard lock(_mutex);

        return picoquic_start_key_rotation(_cnx);
    }

    picoquic_quic_t *ContextBase::get_quic_ctx() {
        std::lock_guard lock(_mutex);

        return picoquic_get_quic_ctx(_cnx);
    }

    picoquic_cnx_t *ContextBase::get_next_cnx() {
        std::lock_guard lock(_mutex);

        return picoquic_get_next_cnx(_cnx);
    }

    int64_t ContextBase::get_wake_delay(uint64_t current_time, int64_t delay_max) {
        std::lock_guard lock(_mutex);

        return picoquic_get_wake_delay(_cnx, current_time, delay_max);
    }

    picoquic_state_enum ContextBase::get_cnx_state() {
        std::lock_guard lock(_mutex);

        return picoquic_get_cnx_state(_cnx);
    }

    void ContextBase::cnx_set_padding_policy(uint32_t padding_multiple, uint32_t padding_minsize) {
        std::lock_guard lock(_mutex);

        picoquic_cnx_set_padding_policy(_cnx, padding_multiple, padding_minsize);
    }

    void ContextBase::cnx_get_padding_policy(uint32_t *padding_multiple, uint32_t *padding_minsize) {
        std::lock_guard lock(_mutex);

        picoquic_cnx_get_padding_policy(_cnx, padding_multiple, padding_minsize);
    }

    void ContextBase::cnx_set_spinbit_policy(picoquic_spinbit_version_enum spinbit_policy) {
        std::lock_guard lock(_mutex);

        picoquic_cnx_set_spinbit_policy(_cnx, spinbit_policy);
    }

    void ContextBase::set_crypto_epoch_length(uint64_t crypto_epoch_length_max) {
        std::lock_guard lock(_mutex);

        picoquic_set_crypto_epoch_length(_cnx, crypto_epoch_length_max);
    }

    uint64_t ContextBase::get_crypto_epoch_length() {
        std::lock_guard lock(_mutex);

        return picoquic_get_crypto_epoch_length(_cnx);
    }

    void ContextBase::cnx_set_pmtud_policy(picoquic_pmtud_policy_enum pmtud_policy) {
        std::lock_guard lock(_mutex);

        picoquic_cnx_set_pmtud_policy(_cnx, pmtud_policy);
    }

    void ContextBase::cnx_set_pmtud_required(int is_pmtud_required) {
        std::lock_guard lock(_mutex);

        picoquic_cnx_set_pmtud_required(_cnx, is_pmtud_required);
    }

    int ContextBase::tls_is_psk_handshake() {
        std::lock_guard lock(_mutex);

        return picoquic_tls_is_psk_handshake(_cnx);
    }

    void ContextBase::get_peer_addr(struct sockaddr **addr) {
        std::lock_guard lock(_mutex);

        picoquic_get_peer_addr(_cnx, addr);
    }

    void ContextBase::get_local_addr(struct sockaddr **addr) {
        std::lock_guard lock(_mutex);

        picoquic_get_local_addr(_cnx, addr);
    }

    unsigned long ContextBase::get_local_if_index() {
        std::lock_guard lock(_mutex);

        return picoquic_get_local_if_index(_cnx);
    }

    int ContextBase::set_local_addr(struct sockaddr *addr) {
        std::lock_guard lock(_mutex);

        return picoquic_set_local_addr(_cnx, addr);
    }

    picoquic_connection_id_t ContextBase::get_local_cnxid() {
        std::lock_guard lock(_mutex);

        return picoquic_get_local_cnxid(_cnx);
    }

    picoquic_connection_id_t ContextBase::get_remote_cnxid() {
        std::lock_guard lock(_mutex);

        return picoquic_get_remote_cnxid(_cnx);
    }

    picoquic_connection_id_t ContextBase::get_initial_cnxid() {
        std::lock_guard lock(_mutex);

        return picoquic_get_initial_cnxid(_cnx);
    }

    picoquic_connection_id_t ContextBase::get_client_cnxid() {
        std::lock_guard lock(_mutex);

        return picoquic_get_client_cnxid(_cnx);
    }

    picoquic_connection_id_t ContextBase::get_server_cnxid() {
        std::lock_guard lock(_mutex);

        return picoquic_get_server_cnxid(_cnx);
    }

    picoquic_connection_id_t ContextBase::get_logging_cnxid() {
        std::lock_guard lock(_mutex);

        return picoquic_get_logging_cnxid(_cnx);
    }

    uint64_t ContextBase::get_cnx_start_time() {
        std::lock_guard lock(_mutex);

        return picoquic_get_cnx_start_time(_cnx);
    }

    uint64_t ContextBase::is_0rtt_available() {
        std::lock_guard lock(_mutex);

        return picoquic_is_0rtt_available(_cnx);
    }

    int ContextBase::is_cnx_backlog_empty() {
        std::lock_guard lock(_mutex);

        return picoquic_is_cnx_backlog_empty(_cnx);
    }

    void ContextBase::set_callback(picoquic_stream_data_cb_fn callback_fn, void *callback_ctx) {
        std::lock_guard lock(_mutex);

        picoquic_set_callback(_cnx, callback_fn, callback_ctx);
    }

    picoquic_stream_data_cb_fn ContextBase::get_callback_function() {
        std::lock_guard lock(_mutex);

        return picoquic_get_callback_function(_cnx);
    }

    void *ContextBase::get_callback_context() {
        std::lock_guard lock(_mutex);

        return picoquic_get_callback_context(_cnx);
    }

    int ContextBase::queue_misc_frame(const uint8_t *bytes, size_t length, int is_pure_ack) {
        std::lock_guard lock(_mutex);

        return picoquic_queue_misc_frame(_cnx, bytes, length, is_pure_ack);
    }

    int ContextBase::queue_datagram_frame(size_t length, const uint8_t *bytes) {
        std::lock_guard lock(_mutex);

        return picoquic_queue_datagram_frame(_cnx, length, bytes);
    }

    int ContextBase::prepare_packet_ex(uint64_t current_time, uint8_t *send_buffer, size_t send_buffer_max,
                                       size_t *send_length, struct sockaddr_storage *p_addr_to,
                                       struct sockaddr_storage *p_addr_from, int *if_index, size_t *send_msg_size) {
        std::lock_guard lock(_mutex);

        return picoquic_prepare_packet_ex(_cnx, current_time, send_buffer, send_buffer_max, send_length, p_addr_to, p_addr_from, if_index, send_msg_size);
    }

    int ContextBase::prepare_packet(uint64_t current_time, uint8_t *send_buffer, size_t send_buffer_max,
                                    size_t *send_length, struct sockaddr_storage *p_addr_to,
                                    struct sockaddr_storage *p_addr_from, int *if_index) {
        std::lock_guard lock(_mutex);

        return picoquic_prepare_packet(_cnx, current_time, send_buffer, send_buffer_max, send_length, p_addr_to, p_addr_from, if_index);
    }

    void ContextBase::notify_destination_unreachable(uint64_t current_time, struct sockaddr *addr_peer,
                                                     struct sockaddr *addr_local, int if_index, int socket_err) {
        std::lock_guard lock(_mutex);

        picoquic_notify_destination_unreachable(_cnx, current_time, addr_peer, addr_local, if_index, socket_err);
    }

    int ContextBase::mark_direct_receive_stream(uint64_t stream_id, picoquic_stream_direct_receive_fn direct_receive_fn,
                                                void *direct_receive_ctx) {
        std::lock_guard lock(_mutex);

        return picoquic_mark_direct_receive_stream(_cnx, stream_id, direct_receive_fn, direct_receive_ctx);
    }

    int ContextBase::set_app_stream_ctx(uint64_t stream_id, void *app_stream_ctx) {
        std::lock_guard lock(_mutex);

        return picoquic_set_app_stream_ctx(_cnx, stream_id, app_stream_ctx);
    }

    void ContextBase::unlink_app_stream_ctx(uint64_t stream_id) {
        std::lock_guard lock(_mutex);

        picoquic_unlink_app_stream_ctx(_cnx, stream_id);
    }

    int ContextBase::mark_active_stream(uint64_t stream_id, int is_active, void *v_stream_ctx) {
        std::lock_guard lock(_mutex);

        return picoquic_mark_active_stream(_cnx, stream_id, is_active, v_stream_ctx);
    }

    int ContextBase::set_stream_priority(uint64_t stream_id, uint8_t stream_priority) {
        std::lock_guard lock(_mutex);

        return picoquic_set_stream_priority(_cnx, stream_id, stream_priority);
    }

    int ContextBase::mark_high_priority_stream(uint64_t stream_id, int is_high_priority) {
        std::lock_guard lock(_mutex);

        return picoquic_mark_high_priority_stream(_cnx, stream_id, is_high_priority);
    }

    int ContextBase::add_to_stream(uint64_t stream_id, const uint8_t *data, size_t length, int set_fin) {
        std::lock_guard lock(_mutex);

        return picoquic_add_to_stream(_cnx, stream_id, data, length, set_fin);
    }

    void ContextBase::reset_stream_ctx(uint64_t stream_id) {
        std::lock_guard lock(_mutex);

        picoquic_reset_stream_ctx(_cnx, stream_id);
    }

    int ContextBase::add_to_stream_with_ctx(uint64_t stream_id, const uint8_t *data, size_t length, int set_fin,
                                            void *app_stream_ctx) {
        std::lock_guard lock(_mutex);

        return picoquic_add_to_stream_with_ctx(_cnx, stream_id, data, length, set_fin, app_stream_ctx);
    }

    int ContextBase::reset_stream(uint64_t stream_id, uint64_t local_stream_error) {
        std::lock_guard lock(_mutex);

        return picoquic_reset_stream(_cnx, stream_id, local_stream_error);
    }

    int ContextBase::open_flow_control(uint64_t stream_id, uint64_t expected_data_size) {
        std::lock_guard lock(_mutex);

        return picoquic_open_flow_control(_cnx, stream_id, expected_data_size);
    }

    uint64_t ContextBase::get_next_local_stream_id(int is_unidir) {
        std::lock_guard lock(_mutex);

        return picoquic_get_next_local_stream_id(_cnx, is_unidir);
    }

    int ContextBase::stop_sending(uint64_t stream_id, uint16_t local_stream_error) {
        std::lock_guard lock(_mutex);

        return picoquic_stop_sending(_cnx, stream_id, local_stream_error);
    }

    int ContextBase::discard_stream(uint64_t stream_id, uint16_t local_stream_error) {
        std::lock_guard lock(_mutex);

        return picoquic_discard_stream(_cnx, stream_id, local_stream_error);
    }

    int ContextBase::mark_datagram_ready(int is_ready) {
        std::lock_guard lock(_mutex);

        return picoquic_mark_datagram_ready(_cnx, is_ready);
    }

    int ContextBase::mark_datagram_ready_path(uint64_t unique_path_id, int is_path_ready) {
        std::lock_guard lock(_mutex);

        return picoquic_mark_datagram_ready_path(_cnx, unique_path_id, is_path_ready);
    }

    void ContextBase::set_preemptive_repeat_per_cnx(int do_repeat) {
        std::lock_guard lock(_mutex);

        picoquic_set_preemptive_repeat_per_cnx(_cnx, do_repeat);
    }

    void ContextBase::enable_keep_alive(uint64_t interval) {
        std::lock_guard lock(_mutex);

        picoquic_enable_keep_alive(_cnx, interval);
    }

    void ContextBase::disable_keep_alive() {
        std::lock_guard lock(_mutex);

        picoquic_disable_keep_alive(_cnx);
    }

    int ContextBase::is_client() {
        std::lock_guard lock(_mutex);

        return picoquic_is_client(_cnx);
    }

    uint64_t ContextBase::get_local_error() {
        std::lock_guard lock(_mutex);

        return picoquic_get_local_error(_cnx);
    }

    uint64_t ContextBase::get_remote_error() {
        std::lock_guard lock(_mutex);

        return picoquic_get_remote_error(_cnx);
    }

    uint64_t ContextBase::get_application_error() {
        std::lock_guard lock(_mutex);

        return picoquic_get_application_error(_cnx);
    }

    uint64_t ContextBase::get_remote_stream_error(uint64_t stream_id) {
        std::lock_guard lock(_mutex);

        return picoquic_get_remote_stream_error(_cnx, stream_id);
    }

    uint64_t ContextBase::get_data_sent() {
        std::lock_guard lock(_mutex);

        return picoquic_get_data_sent(_cnx);
    }

    uint64_t ContextBase::get_data_received() {
        std::lock_guard lock(_mutex);

        return picoquic_get_data_received(_cnx);
    }

    int ContextBase::cnx_is_still_logging() {
        std::lock_guard lock(_mutex);

        return picoquic_cnx_is_still_logging(_cnx);
    }

    void ContextBase::set_congestion_algorithm(const picoquic_congestion_algorithm_t *algo) {
        std::lock_guard lock(_mutex);

        picoquic_set_congestion_algorithm(_cnx, algo);
    }

    void ContextBase::subscribe_pacing_rate_updates(uint64_t decrease_threshold, uint64_t increase_threshold) {
        std::lock_guard lock(_mutex);

        picoquic_subscribe_pacing_rate_updates(_cnx, decrease_threshold, increase_threshold);
    }

    uint64_t ContextBase::get_pacing_rate() {
        std::lock_guard lock(_mutex);

        return picoquic_get_pacing_rate(_cnx);
    }

    uint64_t ContextBase::get_cwin() {
        std::lock_guard lock(_mutex);

        return picoquic_get_cwin(_cnx);
    }

    uint64_t ContextBase::get_rtt() {
        std::lock_guard lock(_mutex);

        return picoquic_get_rtt(_cnx);
    }

    const char *ContextBase::tls_get_negotiated_alpn() {
        std::lock_guard lock(_mutex);

        return picoquic_tls_get_negotiated_alpn(_cnx);
    }

    const char *ContextBase::tls_get_sni() {
        std::lock_guard lock(_mutex);

        return picoquic_tls_get_sni(_cnx);
    }

    int ContextBase::stream_data_cb(picoquic_cnx_t *cnx, uint64_t stream_id, uint8_t *bytes, size_t length,
                                    picoquic_call_back_event_t fin_or_event, void *callback_ctx, void *v_stream_ctx) {
        return Context::stream_data_cb(cnx, stream_id, bytes, length, fin_or_event, callback_ctx, v_stream_ctx);
    }

    int ContextBase::stream_direct_receive(picoquic_cnx_t *cnx, uint64_t stream_id, int fin, const uint8_t *bytes,
                                           uint64_t offset, size_t length, void *direct_receive_ctx) {
        return Context::stream_direct_receive(cnx, stream_id, fin, bytes, offset, length, direct_receive_ctx);
    }

    ContextBase::~ContextBase() {
        delete_cnx();
    }

} // quic