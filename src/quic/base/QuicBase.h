//
// Created by Matthias Hofstätter on 27.08.23.
//

#ifndef MPP_QUICBASE_H
#define MPP_QUICBASE_H

#include <picoquic.h>
#include <picoquic_packet_loop.h>
#include <mutex>

namespace quic {

    class QuicBase {
    public:
        uint64_t get_quic_time();
        void set_fuzz(picoquic_fuzz_fn fuzz_fn, void * fuzz_ctx);
        void set_log_level(int log_level);
        void use_unique_log_names(int use_unique_log_names);
        void set_random_initial(int random_initial);
        void set_packet_train_mode(int train_mode);
        void set_padding_policy(uint32_t padding_min_size, uint32_t padding_multiple);
        void set_key_log_file(char const* keylog_filename);
        int adjust_max_connections(uint32_t max_nb_connections);
        uint32_t current_number_connections();
        void disable_port_blocking(int is_port_blocking_disabled);
        void free();
        int set_low_memory_mode(int low_memory_mode);
        void set_cookie_mode(int cookie_mode);
        int set_cipher_suite(int cipher_suite_id);
        int set_key_exchange(int key_exchange_id);
        void set_tls_certificate_chain(ptls_iovec_t* certs, size_t count);
        int set_tls_root_certificates(ptls_iovec_t* certs, size_t count);
        void set_null_verifier();
        //int set_tls_key(const uint8_t* data, size_t len);
        int set_verify_certificate_callback(ptls_verify_certificate_t * cb, picoquic_free_verify_certificate_ctx free_fn);
        void set_client_authentication(int client_authentication);
        void enforce_client_only(int do_enforce);
        void set_default_padding(uint32_t padding_multiple, uint32_t padding_minsize);
        void set_default_spinbit_policy(picoquic_spinbit_version_enum default_spinbit_policy);
        void set_default_lossbit_policy(picoquic_lossbit_version_enum default_lossbit_policy);
        void set_default_multipath_option(int multipath_option);
        void set_cwin_max(uint64_t cwin_max);
        void set_max_data_control(uint64_t max_data);
        void set_default_idle_timeout(uint64_t idle_timeout);
        void set_default_crypto_epoch_length(uint64_t crypto_epoch_length_max);
        uint64_t get_default_crypto_epoch_length();
        uint8_t get_local_cid_length();
        int is_local_cid(picoquic_connection_id_t* cid);
        int load_retry_tokens(char const* token_store_filename);
        int save_session_tickets(char const* ticket_store_filename);
        int save_retry_tokens(char const* token_store_filename);
        void set_default_bdp_frame_option(int enable_bdp_frame);
        int set_default_connection_id_length(uint8_t cid_length);
        void set_default_connection_id_ttl(uint64_t ttl_usec);
        uint64_t get_default_connection_id_ttl();
        void set_mtu_max(uint32_t mtu_max);
        void set_alpn_select_fn(picoquic_alpn_select_fn alpn_select_fn);
        void set_default_callback(picoquic_stream_data_cb_fn callback_fn, void *callback_ctx);
        void set_default_stateless_reset_min_interval(uint64_t min_interval_usec);
        void set_max_simultaneous_logs(uint32_t max_simultaneous_logs);
        uint32_t get_max_simultaneous_logs();
        picoquic_cnx_t* create_cnx(picoquic_connection_id_t initial_cnx_id, picoquic_connection_id_t remote_cnx_id, const struct sockaddr* addr_to, uint64_t start_time, uint32_t preferred_version, char const* sni, char const* alpn, char client_mode);
        picoquic_cnx_t* create_client_cnx(struct sockaddr* addr, uint64_t start_time, uint32_t preferred_version, char const* sni, char const* alpn, picoquic_stream_data_cb_fn callback_fn, void* callback_ctx);
        void enable_path_callbacks_default(int are_enabled);
        void default_quality_update(uint64_t pacing_rate_delta, uint64_t rtt_delta);
        picoquic_cnx_t* get_first_cnx();
        int64_t get_next_wake_delay(uint64_t current_time, int64_t delay_max);
        picoquic_cnx_t* get_earliest_cnx_to_wake(uint64_t max_wake_time);
        uint64_t get_next_wake_time(uint64_t current_time);
        void set_default_pmtud_policy(picoquic_pmtud_policy_enum pmtud_policy);
        picoquic_stream_data_cb_fn get_default_callback_function();
        void *get_default_callback_context();
        int prepare_next_packet_ex(uint64_t current_time, uint8_t* send_buffer, size_t send_buffer_max, size_t* send_length, struct sockaddr_storage* p_addr_to, struct sockaddr_storage* p_addr_from, int* if_index, picoquic_connection_id_t* log_cid, picoquic_cnx_t** p_last_cnx, size_t* send_msg_size);
        int prepare_next_packet(uint64_t current_time, uint8_t* send_buffer, size_t send_buffer_max, size_t* send_length, struct sockaddr_storage* p_addr_to, struct sockaddr_storage* p_addr_from, int* if_index, picoquic_connection_id_t* p_logcid, picoquic_cnx_t** p_last_cnx);
        void notify_destination_unreachable_by_cnxid(picoquic_connection_id_t * cnxid, uint64_t current_time, struct sockaddr* addr_peer, struct sockaddr* addr_local, int if_index, int socket_err);
        void set_default_priority(uint8_t default_stream_priority);
        void set_optimistic_ack_policy(uint32_t sequence_hole_pseudo_period);
        void set_preemptive_repeat_policy(int do_repeat);
        void set_default_congestion_algorithm(picoquic_congestion_algorithm_t const* algo);
        void set_default_congestion_algorithm_by_name(char const* alg_name);

        // sockloop
        int packet_loop(int local_port, int local_af, int dest_if, int socket_buffer_size, int do_not_use_gso, picoquic_packet_loop_cb_fn loop_callback, void* loop_callback_ctx);

        // picosocks
        void set_key_log_file_from_env();

        // autoqlog
        int set_qlog(char const* qlog_dir);

        // set_binlog
        int set_binlog(char const* binlog_dir);

        virtual ~QuicBase();

    protected:
        picoquic_quic_t *_quic = nullptr;
        std::recursive_mutex _mutex;

        QuicBase(uint32_t max_nb_connections, char const* cert_file_name, char const* key_file_name, char const * cert_root_file_name,
                 char const* default_alpn, picoquic_stream_data_cb_fn default_callback_fn, void* default_callback_ctx,
                 picoquic_connection_id_cb_fn cnx_id_callback, void* cnx_id_callback_data, uint8_t reset_seed[PICOQUIC_RESET_SECRET_SIZE],
                 uint64_t current_time, uint64_t* p_simulated_time, char const* ticket_file_name, const uint8_t* ticket_encryption_key,
                 size_t ticket_encryption_key_length);

    private:
        static size_t alpn_select(ptls_iovec_t* list, size_t count);
        static void connection_id_cb(picoquic_connection_id_t cnx_id_local, picoquic_connection_id_t cnx_id_remote, void* cnx_id_cb_data, picoquic_connection_id_t * cnx_id_returned);
        static uint32_t fuzz(void * fuzz_ctx, picoquic_cnx_t* cnx, uint8_t * bytes, size_t bytes_max, size_t length, size_t header_length);
        static int loop_callback(picoquic_quic_t* quic, picoquic_packet_loop_cb_enum cb_mode, void* callback_ctx, void * callback_arg);
    };

} // base

#endif //MPP_QUICBASE_H
