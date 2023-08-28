//
// Created by Matthias Hofstätter on 27.08.23.
//

#include "QuicBase.h"
#include "../Quic.h"
#include "picoquic_set_binlog.h"

namespace quic {

    QuicBase::QuicBase(uint32_t max_nb_connections, const char *cert_file_name, const char *key_file_name,
                       const char *cert_root_file_name, const char *default_alpn,
                       picoquic_stream_data_cb_fn default_callback_fn, void *default_callback_ctx,
                       picoquic_connection_id_cb_fn cnx_id_callback, void *cnx_id_callback_data, uint8_t *reset_seed,
                       uint64_t current_time, uint64_t *p_simulated_time, const char *ticket_file_name,
                       const uint8_t *ticket_encryption_key, size_t ticket_encryption_key_length) : _quic(picoquic_create(max_nb_connections, cert_file_name, key_file_name, cert_root_file_name, default_alpn, default_callback_fn, default_callback_ctx, cnx_id_callback, cnx_id_callback_data, reset_seed, current_time, p_simulated_time, ticket_file_name, ticket_encryption_key, ticket_encryption_key_length)) {

    }

    uint64_t QuicBase::get_quic_time() {
        std::lock_guard lock(_mutex);

        return picoquic_get_quic_time(_quic);
    }

    void QuicBase::set_fuzz(picoquic_fuzz_fn fuzz_fn, void *fuzz_ctx) {
        std::lock_guard lock(_mutex);

        picoquic_set_fuzz(_quic, fuzz_fn, fuzz_ctx);
    }

    void QuicBase::set_log_level(int log_level) {
        std::lock_guard lock(_mutex);

        picoquic_set_log_level(_quic, log_level);
    }

    void QuicBase::use_unique_log_names(int use_unique_log_names) {
        std::lock_guard lock(_mutex);

        picoquic_use_unique_log_names(_quic, use_unique_log_names);
    }

    void QuicBase::set_random_initial(int random_initial) {
        std::lock_guard lock(_mutex);

        picoquic_set_random_initial(_quic, random_initial);
    }

    void QuicBase::set_packet_train_mode(int train_mode) {
        std::lock_guard lock(_mutex);

        picoquic_set_packet_train_mode(_quic, train_mode);
    }

    void QuicBase::set_padding_policy(uint32_t padding_min_size, uint32_t padding_multiple) {
        std::lock_guard lock(_mutex);

        picoquic_set_padding_policy(_quic, padding_min_size, padding_multiple);
    }

    void QuicBase::set_key_log_file(const char *keylog_filename) {
        std::lock_guard lock(_mutex);

        picoquic_set_key_log_file(_quic, keylog_filename);
    }

    int QuicBase::adjust_max_connections(uint32_t max_nb_connections) {
        std::lock_guard lock(_mutex);

        return picoquic_adjust_max_connections(_quic, max_nb_connections);
    }

    uint32_t QuicBase::current_number_connections() {
        std::lock_guard lock(_mutex);

        return picoquic_current_number_connections(_quic);
    }

    void QuicBase::disable_port_blocking(int is_port_blocking_disabled) {
        std::lock_guard lock(_mutex);

        picoquic_disable_port_blocking(_quic, is_port_blocking_disabled);
    }

    void QuicBase::free() {
        std::lock_guard lock(_mutex);

        picoquic_free(_quic);
    }

    int QuicBase::set_low_memory_mode(int low_memory_mode) {
        std::lock_guard lock(_mutex);

        return picoquic_set_low_memory_mode(_quic, low_memory_mode);
    }

    void QuicBase::set_cookie_mode(int cookie_mode) {
        std::lock_guard lock(_mutex);

        picoquic_set_cookie_mode(_quic, cookie_mode);
    }

    int QuicBase::set_cipher_suite(int cipher_suite_id) {
        std::lock_guard lock(_mutex);

        return picoquic_set_cipher_suite(_quic, cipher_suite_id);
    }

    int QuicBase::set_key_exchange(int key_exchange_id) {
        std::lock_guard lock(_mutex);

        return picoquic_set_key_exchange(_quic, key_exchange_id);
    }

    void QuicBase::set_tls_certificate_chain(ptls_iovec_t *certs, size_t count) {
        std::lock_guard lock(_mutex);

        picoquic_set_tls_certificate_chain(_quic, certs, count);
    }

    int QuicBase::set_tls_root_certificates(ptls_iovec_t *certs, size_t count) {
        std::lock_guard lock(_mutex);

        return picoquic_set_tls_root_certificates(_quic, certs, count);
    }

    void QuicBase::set_null_verifier() {
        std::lock_guard lock(_mutex);

        picoquic_set_null_verifier(_quic);
    }

    /*int QuicBase::set_tls_key(const uint8_t *data, size_t len) {
        std::lock_guard lock(_mutex);

        return picoquic_set_tls_key(_quic, data, len);
    }*/

    int QuicBase::set_verify_certificate_callback(ptls_verify_certificate_t *cb,
                                                  picoquic_free_verify_certificate_ctx free_fn) {
        std::lock_guard lock(_mutex);

        return picoquic_set_verify_certificate_callback(_quic, cb, free_fn);
    }

    void QuicBase::set_client_authentication(int client_authentication) {
        std::lock_guard lock(_mutex);

        picoquic_set_client_authentication(_quic, client_authentication);
    }

    void QuicBase::enforce_client_only(int do_enforce) {
        std::lock_guard lock(_mutex);

        picoquic_enforce_client_only(_quic, do_enforce);
    }

    void QuicBase::set_default_padding(uint32_t padding_multiple, uint32_t padding_minsize) {
        std::lock_guard lock(_mutex);

        picoquic_set_default_padding(_quic, padding_multiple, padding_minsize);
    }

    void
    QuicBase::set_default_spinbit_policy(picoquic_spinbit_version_enum default_spinbit_policy) {
        std::lock_guard lock(_mutex);

        picoquic_set_default_spinbit_policy(_quic, default_spinbit_policy);
    }

    void QuicBase::set_default_lossbit_policy(picoquic_lossbit_version_enum default_lossbit_policy) {
        std::lock_guard lock(_mutex);

        picoquic_set_default_lossbit_policy(_quic, default_lossbit_policy);
    }

    void QuicBase::set_default_multipath_option(int multipath_option) {
        std::lock_guard lock(_mutex);

        picoquic_set_default_multipath_option(_quic, multipath_option);
    }

    void QuicBase::set_cwin_max(uint64_t cwin_max) {
        std::lock_guard lock(_mutex);

        picoquic_set_cwin_max(_quic, cwin_max);
    }

    void QuicBase::set_max_data_control(uint64_t max_data) {
        std::lock_guard lock(_mutex);

        picoquic_set_max_data_control(_quic, max_data);
    }

    void QuicBase::set_default_idle_timeout(uint64_t idle_timeout) {
        std::lock_guard lock(_mutex);

        picoquic_set_default_idle_timeout(_quic, idle_timeout);
    }

    void QuicBase::set_default_crypto_epoch_length(uint64_t crypto_epoch_length_max) {
        std::lock_guard lock(_mutex);

        picoquic_set_default_crypto_epoch_length(_quic, crypto_epoch_length_max);
    }

    uint64_t QuicBase::get_default_crypto_epoch_length() {
        std::lock_guard lock(_mutex);

        return picoquic_get_default_crypto_epoch_length(_quic);
    }

    uint8_t QuicBase::get_local_cid_length() {
        std::lock_guard lock(_mutex);

        return picoquic_get_local_cid_length(_quic);
    }

    int QuicBase::is_local_cid(picoquic_connection_id_t *cid) {
        std::lock_guard lock(_mutex);

        return picoquic_is_local_cid(_quic, cid);
    }

    int QuicBase::load_retry_tokens(const char *token_store_filename) {
        std::lock_guard lock(_mutex);

        return picoquic_load_retry_tokens(_quic, token_store_filename);
    }

    int QuicBase::save_session_tickets(const char *ticket_store_filename) {
        std::lock_guard lock(_mutex);

        return picoquic_save_session_tickets(_quic, ticket_store_filename);
    }

    int QuicBase::save_retry_tokens(const char *token_store_filename) {
        std::lock_guard lock(_mutex);

        return picoquic_save_retry_tokens(_quic, token_store_filename);
    }

    void QuicBase::set_default_bdp_frame_option(int enable_bdp_frame) {
        std::lock_guard lock(_mutex);

        picoquic_set_default_bdp_frame_option(_quic, enable_bdp_frame);
    }

    int QuicBase::set_default_connection_id_length(uint8_t cid_length) {
        std::lock_guard lock(_mutex);

        return picoquic_set_default_connection_id_length(_quic, cid_length);
    }

    void QuicBase::set_default_connection_id_ttl(uint64_t ttl_usec) {
        std::lock_guard lock(_mutex);

        picoquic_set_default_connection_id_ttl(_quic, ttl_usec);
    }

    uint64_t QuicBase::get_default_connection_id_ttl() {
        std::lock_guard lock(_mutex);

        return picoquic_get_default_connection_id_ttl(_quic);
    }

    void QuicBase::set_mtu_max(uint32_t mtu_max) {
        std::lock_guard lock(_mutex);

        picoquic_set_mtu_max(_quic, mtu_max);
    }

    void QuicBase::set_alpn_select_fn(picoquic_alpn_select_fn alpn_select_fn) {
        std::lock_guard lock(_mutex);

        picoquic_set_alpn_select_fn(_quic, alpn_select_fn);
    }

    void
    QuicBase::set_default_callback(picoquic_stream_data_cb_fn callback_fn, void *callback_ctx) {
        std::lock_guard lock(_mutex);

        picoquic_set_default_callback(_quic, callback_fn, callback_ctx);
    }

    void QuicBase::set_default_stateless_reset_min_interval(uint64_t min_interval_usec) {
        std::lock_guard lock(_mutex);

        picoquic_set_default_stateless_reset_min_interval(_quic, min_interval_usec);
    }

    void QuicBase::set_max_simultaneous_logs(uint32_t max_simultaneous_logs) {
        std::lock_guard lock(_mutex);

        picoquic_set_max_simultaneous_logs(_quic, max_simultaneous_logs);
    }

    uint32_t QuicBase::get_max_simultaneous_logs() {
        std::lock_guard lock(_mutex);

        return picoquic_get_max_simultaneous_logs(_quic);
    }

    picoquic_cnx_t *
    QuicBase::create_cnx(picoquic_connection_id_t initial_cnx_id, picoquic_connection_id_t remote_cnx_id,
                         const struct sockaddr *addr_to, uint64_t start_time, uint32_t preferred_version,
                         const char *sni, const char *alpn, char client_mode) {
        std::lock_guard lock(_mutex);

        return picoquic_create_cnx(_quic, initial_cnx_id, remote_cnx_id, addr_to, start_time, preferred_version, sni, alpn, client_mode);
    }

    picoquic_cnx_t *
    QuicBase::create_client_cnx(struct sockaddr *addr, uint64_t start_time, uint32_t preferred_version, const char *sni,
                                const char *alpn, picoquic_stream_data_cb_fn callback_fn, void *callback_ctx) {
        std::lock_guard lock(_mutex);

        return picoquic_create_client_cnx(_quic, addr, start_time, preferred_version, sni, alpn, callback_fn, callback_ctx);
    }

    void QuicBase::enable_path_callbacks_default(int are_enabled) {
        std::lock_guard lock(_mutex);

        picoquic_enable_path_callbacks_default(_quic, are_enabled);
    }

    void QuicBase::default_quality_update(uint64_t pacing_rate_delta, uint64_t rtt_delta) {
        std::lock_guard lock(_mutex);

        picoquic_default_quality_update(_quic, pacing_rate_delta, rtt_delta);
    }

    picoquic_cnx_t *QuicBase::get_first_cnx() {
        std::lock_guard lock(_mutex);

        return picoquic_get_first_cnx(_quic);
    }

    int64_t QuicBase::get_next_wake_delay(uint64_t current_time, int64_t delay_max) {
        std::lock_guard lock(_mutex);

        return picoquic_get_next_wake_delay(_quic, current_time, delay_max);
    }

    picoquic_cnx_t *QuicBase::get_earliest_cnx_to_wake(uint64_t max_wake_time) {
        std::lock_guard lock(_mutex);

        return picoquic_get_earliest_cnx_to_wake(_quic, max_wake_time);
    }

    uint64_t QuicBase::get_next_wake_time(uint64_t current_time) {
        std::lock_guard lock(_mutex);

        return picoquic_get_next_wake_time(_quic, current_time);
    }

    void QuicBase::set_default_pmtud_policy(picoquic_pmtud_policy_enum pmtud_policy) {
        std::lock_guard lock(_mutex);

        picoquic_set_default_pmtud_policy(_quic, pmtud_policy);
    }

    picoquic_stream_data_cb_fn QuicBase::get_default_callback_function() {
        std::lock_guard lock(_mutex);

        return picoquic_get_default_callback_function(_quic);
    }

    void *QuicBase::get_default_callback_context() {
        std::lock_guard lock(_mutex);

        return picoquic_get_default_callback_context(_quic);
    }

    int QuicBase::prepare_next_packet_ex(uint64_t current_time, uint8_t *send_buffer, size_t send_buffer_max,
                                         size_t *send_length, struct sockaddr_storage *p_addr_to,
                                         struct sockaddr_storage *p_addr_from, int *if_index,
                                         picoquic_connection_id_t *log_cid, picoquic_cnx_t **p_last_cnx,
                                         size_t *send_msg_size) {
        std::lock_guard lock(_mutex);

        return picoquic_prepare_next_packet_ex(_quic, current_time, send_buffer, send_buffer_max, send_length, p_addr_to, p_addr_from, if_index, log_cid, p_last_cnx, send_msg_size);
    }

    int QuicBase::prepare_next_packet(uint64_t current_time, uint8_t *send_buffer, size_t send_buffer_max,
                                      size_t *send_length, struct sockaddr_storage *p_addr_to,
                                      struct sockaddr_storage *p_addr_from, int *if_index,
                                      picoquic_connection_id_t *p_logcid, picoquic_cnx_t **p_last_cnx) {
        std::lock_guard lock(_mutex);

        return picoquic_prepare_next_packet(_quic, current_time, send_buffer, send_buffer_max, send_length, p_addr_to, p_addr_from, if_index, p_logcid, p_last_cnx);
    }

    void QuicBase::notify_destination_unreachable_by_cnxid(picoquic_connection_id_t *cnxid, uint64_t current_time,
                                                           struct sockaddr *addr_peer, struct sockaddr *addr_local,
                                                           int if_index, int socket_err) {
        std::lock_guard lock(_mutex);

        picoquic_notify_destination_unreachable_by_cnxid(_quic, cnxid, current_time, addr_peer, addr_local, if_index, socket_err);
    }

    void QuicBase::set_default_priority(uint8_t default_stream_priority) {
        std::lock_guard lock(_mutex);

        picoquic_set_default_priority(_quic, default_stream_priority);
    }

    void QuicBase::set_optimistic_ack_policy(uint32_t sequence_hole_pseudo_period) {
        std::lock_guard lock(_mutex);

        picoquic_set_optimistic_ack_policy(_quic, sequence_hole_pseudo_period);
    }

    void QuicBase::set_preemptive_repeat_policy(int do_repeat) {
        std::lock_guard lock(_mutex);

        picoquic_set_preemptive_repeat_policy(_quic, do_repeat);
    }

    void QuicBase::set_default_congestion_algorithm(const picoquic_congestion_algorithm_t *algo) {
        std::lock_guard lock(_mutex);

        picoquic_set_default_congestion_algorithm(_quic, algo);
    }

    void QuicBase::set_default_congestion_algorithm_by_name(const char *alg_name) {
        std::lock_guard lock(_mutex);

        picoquic_set_default_congestion_algorithm_by_name(_quic, alg_name);
    }

    int QuicBase::packet_loop(int local_port, int local_af, int dest_if,
                                       int socket_buffer_size, int do_not_use_gso,
                                       picoquic_packet_loop_cb_fn loop_callback, void *loop_callback_ctx) {
        return picoquic_packet_loop(_quic, local_port, local_af, dest_if, socket_buffer_size, do_not_use_gso, loop_callback, loop_callback_ctx);
    }

    void QuicBase::set_key_log_file_from_env() {
        std::lock_guard lock(_mutex);

        picoquic_set_key_log_file_from_env(_quic);
    }

    int QuicBase::set_qlog(const char *qlog_dir) {
        std::lock_guard lock(_mutex);

        return picoquic_set_qlog(_quic, qlog_dir);
    }

    int QuicBase::set_binlog(const char *binlog_dir) {
        std::lock_guard lock(_mutex);

        return picoquic_set_binlog(_quic, binlog_dir);
    }

    size_t QuicBase::alpn_select(ptls_iovec_t *list, size_t count) {
        return Quic::alpn_select(list, count);
    }

    void QuicBase::connection_id_cb(picoquic_connection_id_t cnx_id_local, picoquic_connection_id_t cnx_id_remote,
                                    void *cnx_id_cb_data, picoquic_connection_id_t *cnx_id_returned) {
        Quic::connection_id_cb(cnx_id_local, cnx_id_remote, cnx_id_cb_data, cnx_id_returned);
    }

    uint32_t QuicBase::fuzz(void *fuzz_ctx, picoquic_cnx_t *cnx, uint8_t *bytes, size_t bytes_max, size_t length,
                            size_t header_length) {
        return Quic::fuzz(fuzz_ctx, cnx, bytes, bytes_max, length, header_length);
    }

    int QuicBase::loop_callback(picoquic_quic_t *quic, picoquic_packet_loop_cb_enum cb_mode, void *callback_ctx,
                                void *callback_arg) {
        return Quic::loop_callback(quic, cb_mode, callback_ctx, callback_arg);
    }

    QuicBase::~QuicBase() {
        free();
    }
} // quic