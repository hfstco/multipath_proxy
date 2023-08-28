//
// Created by Matthias Hofstätter on 31.07.23.
//

#ifndef MPP_QUIC_H
#define MPP_QUIC_H

#include <cstdint>
#include <future>
#include <array>
#include <unordered_map>

#include "picoquic.h"
#include "picosocks.h"
#include "picoquic_utils.h"
#include "autoqlog.h"
#include "picoquic_packet_loop.h"
#include "base/QuicBase.h"

namespace quic {
    class Context;
    class FlowContext;

    class Quic : public QuicBase {
        friend class QuicBase;
    public:
        /*explicit operator picoquic_quic_t *() const noexcept
        { return _quic; }

        explicit operator picoquic_quic_t *() volatile noexcept
        { return _quic; }*/

        static Quic *make(uint32_t max_nb_connections,
                          char const* cert_file_name, char const* key_file_name, char const * cert_root_file_name,
                          char const* default_alpn,
                          picoquic_stream_data_cb_fn default_callback_fn,
                          void* default_callback_ctx,
                          picoquic_connection_id_cb_fn cnx_id_callback,
                          void* cnx_id_callback_data,
                          uint8_t reset_seed[PICOQUIC_RESET_SECRET_SIZE],
                          uint64_t current_time,
                          uint64_t* p_simulated_time,
                          char const* ticket_file_name,
                          const uint8_t* ticket_encryption_key,
                          size_t ticket_encryption_key_length);
        static Quic *make(uint32_t max_nb_connections, char const* cert_file_name, char const* key_file_name);
        static Quic *make(uint32_t max_nb_connections, char const* ticket_file_name);

        void start_packet_loop(int local_port,
                               int local_af,
                               int dest_if,
                               int socket_buffer_size,
                               int do_not_use_gso);

        template<class CONTEXT = Context>
        CONTEXT *create_context(picoquic_connection_id_t initial_cnx_id, picoquic_connection_id_t remote_cnx_id,
                                const struct sockaddr* addr_to, uint64_t start_time, uint32_t preferred_version,
                                char const* sni, char const* alpn, char client_mode) {
            static_assert(std::is_base_of<Context, CONTEXT>::value); // TODO
            std::lock_guard lock(_mutex);

            auto *cnx = create_cnx(initial_cnx_id, remote_cnx_id, addr_to, start_time, preferred_version, sni, alpn, client_mode);
            CONTEXT *context = (CONTEXT *)_contexts.insert({cnx, new CONTEXT(this, cnx)}).first->second;

            return context;
        }

        template<class CONTEXT = Context>
        CONTEXT *create_context(picoquic_cnx_t *cnx) {
            static_assert(std::is_base_of<Context, CONTEXT>::value); // TODO
            std::lock_guard lock(_mutex);

            CONTEXT *context = (CONTEXT *)_contexts.insert({cnx, new CONTEXT(this, cnx)}).first->second;

            return context;
        }

        template<class CONTEXT = Context>
        CONTEXT *get_context(picoquic_cnx_t *cnx) {
            std::lock_guard lock(_mutex);

            auto it = _contexts.find(cnx);
            return (it != _contexts.end()) ? (CONTEXT *)it->second : nullptr;
        }

        void delete_context(Context *context);

        std::string to_string();

        virtual ~Quic();

    private:
        std::future<int> _packet_loop;
        std::unordered_map<picoquic_cnx_t *, Context *> _contexts;

        Quic(uint32_t max_nb_connections,
                   char const* cert_file_name, char const* key_file_name, char const * cert_root_file_name,
                   char const* default_alpn,
                   picoquic_stream_data_cb_fn default_callback_fn,
                   void* default_callback_ctx,
                   picoquic_connection_id_cb_fn cnx_id_callback,
                   void* cnx_id_callback_data,
                   uint8_t reset_seed[PICOQUIC_RESET_SECRET_SIZE],
                   uint64_t current_time,
                   uint64_t* p_simulated_time,
                   char const* ticket_file_name,
                   const uint8_t* ticket_encryption_key,
                   size_t ticket_encryption_key_length);
        Quic(uint32_t max_nb_connections, char const* cert_file_name, char const* key_file_name);
        Quic(uint32_t max_nb_connections, char const* ticket_file_name);

        static size_t alpn_select(ptls_iovec_t* list, size_t count);
        static void connection_id_cb(picoquic_connection_id_t cnx_id_local, picoquic_connection_id_t cnx_id_remote, void* cnx_id_cb_data, picoquic_connection_id_t * cnx_id_returned);
        static uint32_t fuzz(void * fuzz_ctx, picoquic_cnx_t* cnx, uint8_t * bytes, size_t bytes_max, size_t length, size_t header_length);
        static int loop_callback(picoquic_quic_t* quic, picoquic_packet_loop_cb_enum cb_mode, void* callback_ctx, void * callback_arg);
        static int stream_data_cb(picoquic_cnx_t* cnx, uint64_t stream_id, uint8_t* bytes, size_t length, picoquic_call_back_event_t fin_or_event, void* callback_ctx, void* v_stream_ctx);
    };

} // quic

#endif //MPP_QUIC_H
