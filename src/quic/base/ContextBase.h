//
// Created by Matthias Hofstätter on 27.08.23.
//

#ifndef MPP_CONTEXTBASE_H
#define MPP_CONTEXTBASE_H

#include <mutex>
#include <picoquic.h>

namespace quic {
    class QuicBase;

    class ContextBase {
    public:
        void log_app_message_v(const char* fmt, va_list vargs);
        void log_app_message(const char* fmt, ...);
        void get_close_reasons(uint64_t* local_reason, uint64_t* remote_reason, uint64_t* local_application_reason, uint64_t* remote_application_reason);
        void set_transport_parameters(picoquic_tp_t const * tp);
        picoquic_tp_t const* get_transport_parameters(int get_local);
        int start_client_cnx();
        int close(uint16_t application_reason_code);
        void close_immediate();
        void delete_cnx();
        void set_desired_version(uint32_t desired_version);
        void set_rejected_version(uint32_t rejected_version);
        int probe_new_path(const struct sockaddr* addr_from, const struct sockaddr* addr_to, uint64_t current_time);
        int probe_new_path_ex(const struct sockaddr* addr_from, const struct sockaddr* addr_to, int if_index, uint64_t current_time, int to_preferred_address);
        void enable_path_callbacks(int are_enabled);
        int set_app_path_ctx(uint64_t unique_path_id, void * app_path_ctx);
        int abandon_path(uint64_t unique_path_id, uint64_t reason, char const* phrase);
        int refresh_path_connection_id(uint64_t unique_path_id);
        int set_stream_path_affinity(uint64_t stream_id, uint64_t unique_path_id);
        int get_path_quality(uint64_t unique_path_id, picoquic_path_quality_t * quality);
        void get_default_path_quality(picoquic_path_quality_t* quality);
        int subscribe_to_quality_update_per_path(uint64_t unique_path_id, uint64_t pacing_rate_delta, uint64_t rtt_delta);
        void subscribe_to_quality_update(uint64_t pacing_rate_delta, uint64_t rtt_delta);
        int start_key_rotation();
        picoquic_quic_t* get_quic_ctx();
        picoquic_cnx_t* get_next_cnx();
        int64_t get_wake_delay(uint64_t current_time, int64_t delay_max);
        picoquic_state_enum get_cnx_state();
        void cnx_set_padding_policy(uint32_t padding_multiple, uint32_t padding_minsize);
        void cnx_get_padding_policy(uint32_t * padding_multiple, uint32_t * padding_minsize);
        void cnx_set_spinbit_policy(picoquic_spinbit_version_enum spinbit_policy);
        void set_crypto_epoch_length(uint64_t crypto_epoch_length_max);
        uint64_t get_crypto_epoch_length();
        void cnx_set_pmtud_policy(picoquic_pmtud_policy_enum pmtud_policy);
        void cnx_set_pmtud_required(int is_pmtud_required);
        int tls_is_psk_handshake();
        void get_peer_addr(struct sockaddr** addr);
        void get_local_addr(struct sockaddr** addr);
        unsigned long get_local_if_index();
        int set_local_addr(struct sockaddr* addr);
        picoquic_connection_id_t get_local_cnxid();
        picoquic_connection_id_t get_remote_cnxid();
        picoquic_connection_id_t get_initial_cnxid();
        picoquic_connection_id_t get_client_cnxid();
        picoquic_connection_id_t get_server_cnxid();
        picoquic_connection_id_t get_logging_cnxid();
        uint64_t get_cnx_start_time();
        uint64_t is_0rtt_available();
        int is_cnx_backlog_empty();
        void set_callback(picoquic_stream_data_cb_fn callback_fn, void* callback_ctx);
        picoquic_stream_data_cb_fn get_callback_function();
        void * get_callback_context();
        int queue_misc_frame(const uint8_t* bytes, size_t length, int is_pure_ack);
        int queue_datagram_frame(size_t length, const uint8_t* bytes);
        int prepare_packet_ex(uint64_t current_time, uint8_t* send_buffer, size_t send_buffer_max, size_t* send_length, struct sockaddr_storage* p_addr_to, struct sockaddr_storage* p_addr_from, int* if_index, size_t* send_msg_size);
        int prepare_packet(uint64_t current_time, uint8_t* send_buffer, size_t send_buffer_max, size_t* send_length, struct sockaddr_storage* p_addr_to, struct sockaddr_storage* p_addr_from, int* if_index);
        void notify_destination_unreachable(uint64_t current_time, struct sockaddr* addr_peer, struct sockaddr* addr_local, int if_index, int socket_err);
        int mark_direct_receive_stream(uint64_t stream_id, picoquic_stream_direct_receive_fn direct_receive_fn, void* direct_receive_ctx);
        int set_app_stream_ctx(uint64_t stream_id, void* app_stream_ctx);
        void unlink_app_stream_ctx(uint64_t stream_id);
        int mark_active_stream(uint64_t stream_id, int is_active, void* v_stream_ctx);
        int set_stream_priority(uint64_t stream_id, uint8_t stream_priority);
        int mark_high_priority_stream(uint64_t stream_id, int is_high_priority);
        int add_to_stream(uint64_t stream_id, const uint8_t* data, size_t length, int set_fin);
        void reset_stream_ctx(uint64_t stream_id);
        int add_to_stream_with_ctx(uint64_t stream_id, const uint8_t * data, size_t length, int set_fin, void * app_stream_ctx);
        int reset_stream(uint64_t stream_id, uint64_t local_stream_error);
        int open_flow_control(uint64_t stream_id, uint64_t expected_data_size);
        uint64_t get_next_local_stream_id(int is_unidir);
        int stop_sending(uint64_t stream_id, uint16_t local_stream_error);
        int discard_stream(uint64_t stream_id, uint16_t local_stream_error);
        int mark_datagram_ready(int is_ready);
        int mark_datagram_ready_path(uint64_t unique_path_id, int is_path_ready);
        void set_preemptive_repeat_per_cnx(int do_repeat);
        void enable_keep_alive(uint64_t interval);
        void disable_keep_alive();
        int is_client();
        uint64_t get_local_error();
        uint64_t get_remote_error();
        uint64_t get_application_error();
        uint64_t get_remote_stream_error(uint64_t stream_id);
        uint64_t get_data_sent();
        uint64_t get_data_received();
        int cnx_is_still_logging();
        void set_congestion_algorithm(picoquic_congestion_algorithm_t const* algo);
        void subscribe_pacing_rate_updates(uint64_t decrease_threshold, uint64_t increase_threshold);
        uint64_t get_pacing_rate();
        uint64_t get_cwin();
        uint64_t get_rtt();

        // tls api
        const char *tls_get_negotiated_alpn();
        const char *tls_get_sni();

        virtual ~ContextBase();

    protected:
        QuicBase *_quic = nullptr;
        picoquic_cnx_t *_cnx = nullptr;
        std::recursive_mutex _mutex;

        ContextBase(QuicBase *quic, picoquic_cnx_t *cnx);

    private:
        static int stream_data_cb(picoquic_cnx_t* cnx, uint64_t stream_id, uint8_t* bytes, size_t length, picoquic_call_back_event_t fin_or_event, void* callback_ctx, void* v_stream_ctx);
        static int stream_direct_receive(picoquic_cnx_t* cnx, uint64_t stream_id, int fin, const uint8_t* bytes, uint64_t offset, size_t length, void* direct_receive_ctx);
    };

} // quic

#endif //MPP_CONTEXTBASE_H
