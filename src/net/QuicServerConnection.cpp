//
// Created by Matthias Hofstätter on 31.07.23.
//

#include "QuicServerConnection.h"

#include <glog/logging.h>
#include <picoquic_packet_loop.h>

namespace net {
    QuicServerConnection::QuicServerConnection(int port) : QuicConnection(), _port(port) {
        LOG(ERROR) << "QuicServerConnection::QuicServerConnection(" << port << ")";

        _quic = picoquic_create(8, "/root/cert.pem", "/root/key.pem", nullptr, PICOQUIC_SAMPLE_ALPN, &QuicServerConnection::callback,
                                this, nullptr, nullptr, nullptr, _current_time, nullptr, nullptr, nullptr,
                                0);

        picoquic_set_cookie_mode(_quic, 2);
        picoquic_set_default_congestion_algorithm(_quic, picoquic_bbr_algorithm);
        picoquic_set_qlog(_quic, "./qlog");
        picoquic_set_log_level(_quic, 1);
        picoquic_set_key_log_file_from_env(_quic);
        _quic_cnx = picoquic_get_first_cnx(_quic);

        _packet_loop = std::async([this]{
            return picoquic_packet_loop(_quic, _port, 0, 0, 0, 0, &QuicServerConnection::loop_callback, this);
        });
    }

    int QuicServerConnection::loop_callback(picoquic_quic_t *quic, picoquic_packet_loop_cb_enum cb_mode,
                             void *callback_ctx, void *callback_arg) {
        LOG(INFO) << "QuicServerConnection::loop_callback(quic=" << quic << ", cb_mode=" << cb_mode << ", callback_ctx=" << callback_ctx << ", callback_arg=" << callback_arg << ")";

        int ret = 0;
        auto* quicServerConnection = (QuicServerConnection*)callback_ctx;

        if (quicServerConnection == NULL) {
            ret = PICOQUIC_ERROR_UNEXPECTED_ERROR;
        }
        else {
            switch (cb_mode) {
                case picoquic_packet_loop_ready:
                    LOG(INFO) << "picoquic_packet_loop_ready";
                    LOG(INFO) << "Waiting for packets.";
                    //fprintf(stdout, "Waiting for packets.\n");
                    break;
                case picoquic_packet_loop_after_receive:
                    LOG(INFO) << "picoquic_packet_loop_after_receive";
                    break;
                case picoquic_packet_loop_after_send:
                    LOG(INFO) << "picoquic_packet_loop_after_send";
                    break;
                case picoquic_packet_loop_port_update:
                    LOG(INFO) << "picoquic_packet_loop_port_update";
                    break;
                default:
                    LOG(INFO) << "default";
                    ret = PICOQUIC_ERROR_UNEXPECTED_ERROR;
                    break;
            }

            /*if (ret == 0 && cb_ctx->just_once){
                if (!cb_ctx->first_connection_seen && picoquic_get_first_cnx(quic) != NULL) {
                    cb_ctx->first_connection_seen = 1;
                    fprintf(stdout, "First connection noticed.\n");
                } else if (cb_ctx->first_connection_seen && picoquic_get_first_cnx(quic) == NULL) {
                    fprintf(stdout, "No more active connections.\n");
                    cb_ctx->connection_done = 1;
                    ret = PICOQUIC_NO_ERROR_TERMINATE_PACKET_LOOP;
                }
            }*/
        }
        return ret;
    }

    int QuicServerConnection::callback(picoquic_cnx_t* cnx,
                                       uint64_t stream_id, uint8_t* bytes, size_t length,
                                       picoquic_call_back_event_t fin_or_event, void* callback_ctx, void* v_stream_ctx) {
        LOG(ERROR) << "QuicClientConnection::callback(cnx=" << cnx << ", stream_id=" << stream_id << ", bytes=[skip], length=" << length << ", fin_or_event=" << fin_or_event << ", callback_ctx=" << callback_ctx << ", v_stream_ctv=" << v_stream_ctx << ")";

        int ret = 0;
        auto* quicServerConnection = (QuicServerConnection*)callback_ctx;
        auto* quicStream = (QuicStream*)v_stream_ctx;

        /* If this is the first reference to the connection, the application context is set
         * to the default value defined for the server. This default value contains the pointer
         * to the file directory in which all files are defined.
         */
        /*if (callback_ctx == nullptr || callback_ctx == picoquic_get_default_callback_context(picoquic_get_quic_ctx(cnx))) {
            server_ctx = (server_ctx_t *)malloc(sizeof(server_ctx_t));
            if (server_ctx == nullptr) {
                *//* cannot handle the connection *//*
                picoquic_close(cnx, PICOQUIC_ERROR_MEMORY);
                return -1;
            }
            else {
                server_ctx_t* d_ctx = (server_ctx_t*)picoquic_get_default_callback_context(picoquic_get_quic_ctx(cnx));
                if (d_ctx != nullptr) {
                    memcpy(server_ctx, d_ctx, sizeof(server_ctx_t));
                }
                else {
                    *//* This really is an error case: the default connection context should never be nullptr *//*
                    memset(server_ctx, 0, sizeof(server_ctx_t));
                    server_ctx->default_dir = "";
                }
                picoquic_set_callback(cnx, callback, server_ctx);
            }
        }*/

        switch (fin_or_event) {
            case picoquic_callback_stream_data:
            case picoquic_callback_stream_fin:
                /* Data arrival on stream #x, maybe with fin mark */
                if (quicStream == nullptr) {
                    /* Create and initialize stream context */
                    quicStream = quicServerConnection->CreateStream(stream_id);
                    if (picoquic_set_app_stream_ctx(cnx, stream_id, quicStream) != 0) {
                        /* Internal error */
                        (void) picoquic_reset_stream(cnx, stream_id, PICOQUIC_INTERNAL_ERROR);
                        return (-1);
                    }
                }

                if (quicStream == nullptr) {
                    /* Internal error */
                    (void) picoquic_reset_stream(cnx, stream_id, PICOQUIC_INTERNAL_ERROR);
                    return (-1);
                } else {
                    if (fin_or_event == picoquic_callback_stream_fin) {
                        int stream_ret = 0;

                        /* If fin, mark read, check the file, open it. Or reset if there is no such file */
                        //stream_ret = open_stream(quicServerConnection, quicStream);

                        if (stream_ret == 0) {
                            /* If data needs to be sent, set the context as active */
                            ret = picoquic_mark_active_stream(cnx, stream_id, 1, quicStream);
                        } else {
                            /* If the file could not be read, reset the stream */
                            quicServerConnection->CloseStream(quicStream);
                            (void) picoquic_reset_stream(cnx, stream_id, stream_ret);
                        }
                    }
                }
                break;
            case picoquic_callback_prepare_to_send:
                /* Active sending API */
                if (quicStream == nullptr) {
                    /* This should never happen */
                } else {
                    /* Implement the zero copy callback */
                    uint8_t *buffer;

                    buffer = picoquic_provide_stream_data_buffer(bytes, 8, 0, !0); // TODO nb_bytes, is_fin, !is_fin
                    if (buffer != nullptr) {
                        LOG(ERROR) << "buffer error.";
                    } else {
                        /* Should never happen according to callback spec. */
                        LOG(ERROR) << "Should never happen according to callback spec.";
                        ret = -1;
                    }
                }
                break;
            case picoquic_callback_stream_reset: /* Client reset stream #x */
            case picoquic_callback_stop_sending: /* Client asks server to reset stream #x */
                if (quicStream != nullptr) {
                    /* Mark stream as abandoned, close the file, etc. */
                    quicServerConnection->CloseStream(quicStream);
                    picoquic_reset_stream(cnx, stream_id, PICOQUIC_FILE_CANCEL_ERROR);
                }
                break;
            case picoquic_callback_stateless_reset: /* Received an error message */
            case picoquic_callback_close: /* Received connection close */
            case picoquic_callback_application_close: /* Received application close */
                /* Delete the server application context */
                //delete quicServerConnection; // TODO
                picoquic_set_callback(cnx, nullptr, nullptr);
                break;
            case picoquic_callback_version_negotiation:
                /* The server should never receive a version negotiation response */
                break;
            case picoquic_callback_stream_gap:
                /* This callback is never used. */
                break;
            case picoquic_callback_almost_ready:
            case picoquic_callback_ready:
                /* Check that the transport parameters are what the sample expects */
                break;
            default:
                /* unexpected */
                break;
        }

        return ret;
    }

    std::string QuicServerConnection::ToString() {
        return "QuicServerConnection[]";
    }

    QuicServerConnection::~QuicServerConnection() {
        int ret = _packet_loop.get();

        LOG(ERROR) << "QuicServerConnection::~QuicServerConnection()";
    }
}
