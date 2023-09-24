//
// Created by Matthias Hofstätter on 31.07.23.
//

#include "Quic.h"

#include <glog/logging.h>
#include <cassert>
#include <future>
#include <picoquic.h>
#include <picoquic_internal.h>

#include "Stream.h"
#include "../flow/Flow.h"
#include "Context.h"
#include "FlowContext.h"

namespace quic {

    Quic::Quic(uint32_t max_nb_connections,
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
               size_t ticket_encryption_key_length) : QuicBase(max_nb_connections, cert_file_name, key_file_name, cert_file_name, default_alpn, default_callback_fn, default_callback_ctx, cnx_id_callback, cnx_id_callback_data, reset_seed, current_time,
                                                                            p_simulated_time, ticket_file_name, ticket_encryption_key, ticket_encryption_key_length) {
        VLOG(1) << "Quic()"; // TODO
    }

    Quic::Quic(uint32_t max_nb_connections, const char *cert_file_name, const char *key_file_name) : Quic(max_nb_connections, cert_file_name, key_file_name,
                                                                                                          nullptr, QUIC_ALPN_FLOW,
                                                                                                          stream_data_cb, this,
                                                                                                          nullptr, nullptr, nullptr, picoquic_current_time(),
                                                                                                          nullptr, nullptr, nullptr, 0) {
        VLOG(1) << "Quic()"; // TODO
    }

    Quic::Quic(uint32_t max_nb_connections, const char *ticket_file_name) : Quic(max_nb_connections, nullptr, nullptr,
                                                                                 nullptr, QUIC_ALPN_FLOW, nullptr,
                                                                                 nullptr, nullptr, nullptr, nullptr, picoquic_current_time(),
                                                                                 nullptr, ticket_file_name, nullptr, 0) {
        VLOG(1) << "Quic()"; // TODO
    }

    Quic *Quic::make(uint32_t max_nb_connections, const char *cert_file_name, const char *key_file_name,
                     const char *cert_root_file_name, const char *default_alpn,
                     picoquic_stream_data_cb_fn default_callback_fn, void *default_callback_ctx,
                     picoquic_connection_id_cb_fn cnx_id_callback, void *cnx_id_callback_data, uint8_t *reset_seed,
                     uint64_t current_time, uint64_t *p_simulated_time, const char *ticket_file_name,
                     const uint8_t *ticket_encryption_key, size_t ticket_encryption_key_length) {
        return new Quic(max_nb_connections, cert_file_name, key_file_name, cert_file_name, default_alpn, default_callback_fn, default_callback_ctx, cnx_id_callback, cnx_id_callback_data, reset_seed, current_time, p_simulated_time, ticket_file_name, ticket_encryption_key, ticket_encryption_key_length);
    }

    Quic *Quic::make(uint32_t max_nb_connections, const char *cert_file_name, const char *key_file_name) {
        return new Quic(max_nb_connections, cert_file_name, key_file_name);
    }

    Quic *Quic::make(uint32_t max_nb_connections, const char *ticket_file_name) {
        return new Quic(max_nb_connections, ticket_file_name);
    }

    void Quic::start_packet_loop(int local_port, int local_af, int dest_if, int socket_buffer_size, int do_not_use_gso) {
        _packet_loop = std::async(std::launch::async,[this, local_port, local_af, dest_if, socket_buffer_size, do_not_use_gso] {
            return packet_loop(local_port, local_af, dest_if, socket_buffer_size, do_not_use_gso, &Quic::loop_callback, this);
        });
    }

    void Quic::delete_context(Context *context) {
        std::lock_guard lock(_mutex);

        auto it = _contexts.find((picoquic_cnx_t *)context);

        if (it != _contexts.end()) {
            delete it->second;
            it->second = nullptr;
            _contexts.erase(it);
        }
    }

    size_t Quic::alpn_select(ptls_iovec_t *list, size_t count) {
        assert(false);
        return 0;
    }

    void Quic::connection_id_cb(picoquic_connection_id_t cnx_id_local, picoquic_connection_id_t cnx_id_remote,
                                void *cnx_id_cb_data, picoquic_connection_id_t *cnx_id_returned) {
        assert(false);
    }

    uint32_t Quic::fuzz(void *fuzz_ctx, picoquic_cnx_t *cnx, uint8_t *bytes, size_t bytes_max, size_t length,
                        size_t header_length) {
        assert(false);
        return 0;
    }

    int Quic::loop_callback(picoquic_quic_t *quic, picoquic_packet_loop_cb_enum cb_mode, void *callback_ctx,
                            void *callback_arg) {
        int ret = 0;

        Quic *context = (Quic *) callback_ctx;

        switch (cb_mode) {
            case picoquic_packet_loop_ready: {
                //VLOG(3) << context->to_string() << " picoquic_packet_loop_ready";

                // enable do_time_check
                auto packet_loop_options = (picoquic_packet_loop_options_t *) callback_arg;
                int enable = 1;
                packet_loop_options->do_time_check = enable;

                break;
            }
            case picoquic_packet_loop_after_receive: {
                //VLOG(3) << context->to_string() << " picoquic_packet_loop_after_receive";
                auto bytes_received = *((size_t*)callback_arg);

                break;
            }
            case picoquic_packet_loop_after_send: {
                //VLOG(3) << context->to_string() << " picoquic_packet_loop_after_send";
                auto bytes_sent = *((size_t*)callback_arg);
                break;
            }
            case picoquic_packet_loop_port_update: {
                //VLOG(3) << context->to_string() << " picoquic_packet_loop_port_update";
                auto sockaddr = (struct sockaddr*)callback_arg;

                // update every path[0]
                /*{
                    std::lock_guard lock(context->_mutex); // TODO refactor recursive mutex

                    for (auto &it : context->_contexts) {
                        it.second->_paths.find(0)->second
                    }
                }*/
                break;
            }
            case picoquic_packet_loop_time_check: {
                //VLOG(3) << context->to_string() << " picoquic_packet_loop_time_check";

                // force delta_t 2 us
                auto time_check_arg = (packet_loop_time_check_arg_t *) callback_arg;
                //if (time_check_arg->delta_t > 2) {
                //time_check_arg->delta_t = 0;
                //}

                break;
            }
        }

        return 0;
    }

    int Quic::stream_data_cb(picoquic_cnx_t *cnx, uint64_t stream_id, uint8_t *bytes, size_t length,
                             picoquic_call_back_event_t fin_or_event, void *callback_ctx, void *v_stream_ctx) {
        // new connection
        int ret = 0;

        auto* quic = (Quic *)callback_ctx;
        const char* alpn = cnx->alpn;

        if (strcmp(cnx->alpn, "flow") == 0) {
            quic->create_context<FlowContext>(cnx);
        }

        return ret;
    }

    std::string Quic::to_string() {
        return "Quic[]";
    }

    Quic::~Quic() {
        std::lock_guard lock(_mutex);

        // delete contexts
        for (auto &it : _contexts) {
            delete it.second;
        }
    }
} // quic