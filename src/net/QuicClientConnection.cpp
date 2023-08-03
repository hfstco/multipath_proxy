//
// Created by Matthias Hofstätter on 31.07.23.
//

#include <utility>

#include "QuicClientConnection.h"

#include <glog/logging.h>
#include <picoquic_utils.h>

#include "QuicStream.h"

namespace net {
    QuicClientConnection::QuicClientConnection(std::string server_name, int port) : QuicConnection(), _server_name(server_name), _port(port) {
        LOG(ERROR) << "QuicClientConnection::QuicClientConnection(" << server_name << ", " << port << ")";

        int ret = 0;

        /* Get the server's address */
        if (ret == 0) {
            int is_name = 0;

            ret = picoquic_get_server_address(_server_name.c_str(), _port, &_server_address, &is_name);
            if (ret != 0) {
                LOG(ERROR) << "Cannot get the IP address for " << _server_name << " port " << _port;
                //fprintf(stderr, "Cannot get the IP address for <%s> port <%d>", _server_name.c_str(), _port);
            }
            else if (is_name) {
                _sni = _server_name.c_str();
            }
        }

        /* Create a QUIC context. It could be used for many connections, but in this sample we
         * will use it for just one connection.
         * The sample code exercises just a small subset of the QUIC context configuration options:
         * - use files to store tickets and tokens in order to manage retry and 0-RTT
         * - set the congestion control algorithm to BBR
         * - enable logging of encryption keys for wireshark debugging.
         * - instantiate a binary log option, and log all packets.
         */
        if (ret == 0) {
            _quic = picoquic_create(1, nullptr, nullptr, nullptr, PICOQUIC_SAMPLE_ALPN, nullptr, nullptr,
                                   nullptr, nullptr, nullptr, _current_time, nullptr,
                                   _ticket_store_filename.c_str(), nullptr, 0);

            if (_quic == nullptr) {
                LOG(ERROR) << "Could not create quic context";
                //fprintf(stderr, "Could not create quic context\n");
                ret = -1;
            }
            else {
                if (picoquic_load_retry_tokens(_quic, _token_store_filename.c_str()) != 0) {
                    LOG(ERROR) << "No token file present. Will create one as " << _token_store_filename << ".";
                    //fprintf(stderr, "No token file present. Will create one as <%s>.\n", _token_store_filename.c_str());
                }

                picoquic_set_default_congestion_algorithm(_quic, picoquic_bbr_algorithm);

                picoquic_set_key_log_file_from_env(_quic);
                picoquic_set_qlog(_quic, "./qlog");
                picoquic_set_log_level(_quic, 1);
                // development/debugging, otherwise error 1075 IDLE_TIMEOUT, sender.c:4271 picoquic_check_idle_timer(...)
                picoquic_set_default_idle_timeout(_quic, 1000000000);
            }
        }

        /* Initialize the callback context and create the connection context.
         * We use minimal options on the client side, keeping the transport
         * parameter values set by default for picoquic. This could be fixed later.
         */

        if (ret == 0) {
            //_client_ctx.default_dir = default_dir;
            //_client_ctx.file_names = file_names;
            //_client_ctx.nb_files = nb_files;

            LOG(INFO) << "Starting connection to " << _server_name << ", port " << _port;
            //printf("Starting connection to %s, port %d\n", _server_name.c_str(), _port);

            /* Create a client connection */
            _quic_cnx = picoquic_create_cnx(_quic, picoquic_null_connection_id, picoquic_null_connection_id,
                                            (struct sockaddr*) &_server_address, _current_time, 0, _sni.c_str(), PICOQUIC_SAMPLE_ALPN, 1);

            if (_quic_cnx == nullptr) {
                LOG(ERROR) << "Could not create connection context";
                //fprintf(stderr, "Could not create connection context\n");
                ret = -1;
            }
            else {
                /* Set the client callback context */
                picoquic_set_callback(_quic_cnx, &QuicClientConnection::callback, this);
                /* Client connection parameters could be set here, before starting the connection. */
                ret = picoquic_start_client_cnx(_quic_cnx);
                if (ret < 0) {
                    LOG(ERROR) << "Could not activate connection";
                    //fprintf(stderr, "Could not activate connection\n");
                } else {
                    /* Printing out the initial CID, which is used to identify log files */
                    picoquic_connection_id_t icid = picoquic_get_initial_cnxid(_quic_cnx);
                    LOG(INFO) << "Initial connection ID: " << icid.id;
                    //printf("Initial connection ID: ");
                    /*for (uint8_t i = 0; i < icid.id_len; i++) {
                        printf("%02x", icid.id[i]);
                    }
                    printf("\n");*/
                }
            }

            /* Create a stream context for all the files that should be downloaded */
            /*for (int i = 0; ret == 0 && i < _client_ctx.nb_files; i++) {
                ret = create_stream(cnx, &_client_ctx, i);
                if (ret < 0) {
                    fprintf(stderr, "Could not initiate stream for fi\n");
                }
            }*/
            /*ret = picoquic_mark_active_stream(_quic_cnx, 0, 1, this);
            if (ret != 0) {
                LOG(ERROR) << "Error " << ret << ", cannnot initialize stream.";
                //fprintf(stdout, "Error %d, cannot initialize stream for file number %d\n", ret, (int)file_rank);
            }
            else {
                LOG(INFO) << "Opened stream " << 0;
                //printf("Opened stream %d for file %s\n", 4 * file_rank, client_ctx->file_names[file_rank]);
            }
            CreateStream(0);*/
        }

        /* Wait for packets */
        _packet_loop = std::async(std::launch::async,[this] {
            return picoquic_packet_loop(_quic, 0, _server_address.ss_family, 0, 0, 0, &QuicClientConnection::loop_callback, this);
        });

        /* Done. At this stage, we could print out statistics, etc. */
        //Report(&_client_ctx);

        /* Save tickets and tokens, and free the QUIC context */
        /*if (_quic != nullptr) {
            if (picoquic_save_session_tickets(_quic, ticket_store_filename) != 0) {
                fprintf(stderr, "Could not store the saved session tickets.\n");
            }
            if (picoquic_save_retry_tokens(_quic, token_store_filename) != 0) {
                fprintf(stderr, "Could not save tokens to <%s>.\n", token_store_filename);
            }
            picoquic_free(_quic);
        }*/
    }

    /*void QuicClientConnection::Report(st_client_ctx_t *client_ctx) {
        st_client_stream_ctx_t* stream_ctx = client_ctx->first_stream;

        while (stream_ctx != nullptr) {
            char const* status;
            if (stream_ctx->is_stream_finished) {
                status = "complete";
            }
            else if (stream_ctx->is_stream_reset) {
                status = "reset";
            }
            else {
                status = "unknown status";
            }
            printf("%s: %s, received %zu bytes", client_ctx->file_names[stream_ctx->file_rank], status, stream_ctx->bytes_received);
            if (stream_ctx->is_stream_reset && stream_ctx->remote_error != PICOQUIC_NO_ERROR){
                char const* error_text = "unknown error";
                switch (stream_ctx->remote_error) {
                    case PICOQUIC_INTERNAL_ERROR:
                        error_text = "internal error";
                        break;
                    case PICOQUIC_NAME_TOO_LONG_ERROR:
                        error_text = "internal error";
                        break;
                    case PICOQUIC_NO_SUCH_FILE_ERROR:
                        error_text = "no such file";
                        break;
                    case PICOQUIC_FILE_READ_ERROR:
                        error_text = "file read error";
                        break;
                    case PICOQUIC_FILE_CANCEL_ERROR:
                        error_text = "cancelled";
                        break;
                    default:
                        break;
                }
                printf(", error 0x%" PRIx64 " -- %s", stream_ctx->remote_error, error_text);
            }
            printf("\n");
            stream_ctx = stream_ctx->next_stream;
        }
    }*/

    int QuicClientConnection::callback(picoquic_cnx_t *cnx, uint64_t stream_id, uint8_t *bytes, size_t length,
                                       picoquic_call_back_event_t fin_or_event, void *callback_ctx,
                                       void *v_stream_ctx) {
        int ret = 0;
        auto* quicClientConnection = (QuicClientConnection*)callback_ctx;
        auto* quicStream = (QuicStream*)v_stream_ctx;

        LOG(ERROR) << "QuicClientConnection::callback(cnx=" << cnx << ", stream_id=" << stream_id << ", bytes=[skip], length=" << length << ", fin_or_event=" << fin_or_event << ", callback_ctx=" << callback_ctx << ", v_stream_ctv=" << v_stream_ctx << ")";

        if (quicClientConnection == nullptr) {
            /* This should never happen, because the callback context for the client is initialized
             * when creating the client connection. */
            return -1;
        }

        if (ret == 0) {
            switch (fin_or_event) {
                case picoquic_callback_stream_data:
                    LOG(INFO) << "picoquic_callback_stream_data";
                case picoquic_callback_stream_fin:
                    LOG(INFO) << "picoquic_callback_stream_fin";
                    /* Data arrival on stream #x, maybe with fin mark */
                    if (quicClientConnection == nullptr) {
                        /* This is unexpected, as all contexts were declared when initializing the
                         * connection. */
                        return -1;
                    }
                    else if (quicStream->reset().test() || quicStream->finished().test()) {
                        /* Unexpected: receive after fin */
                        return -1;
                    }
                    else
                    {
                        if (ret == 0 && length > 0) {
                            /* write the received bytes to the file */
                            unsigned char buffer[length];
                            memcpy(buffer, bytes, length);
                            LOG(INFO) << "RECV " << buffer;
                        }

                        if (ret == 0 && fin_or_event == picoquic_callback_stream_fin) {
                            /* everything is done, close the connection */
                            ret = picoquic_close(cnx, 0);
                        }
                    }
                    break;
                case picoquic_callback_stop_sending: /* Should not happen, treated as reset */
                    LOG(INFO) << "picoquic_callback_stop_sending";
                    /* Mark stream as abandoned, close the file, etc. */
                    picoquic_reset_stream(cnx, quicStream->id(), 0);
                    delete quicStream;
                    /* Fall through */
                case picoquic_callback_stream_reset: /* Server reset stream #x */
                    LOG(INFO) << "picoquic_callback_stream_reset";
                    if (quicStream == nullptr) {
                        /* This is unexpected, as all contexts were declared when initializing the
                         * connection. */
                        return -1;
                    }
                    else if (quicStream->reset().test() || quicStream->finished().test()) {
                        /* Unexpected: receive after fin */
                        return -1;
                    }
                    else {
                        quicStream->remoteError(picoquic_get_remote_stream_error(cnx, stream_id));
                        quicStream->reset(true);

                        LOG(INFO) << "All done.";
                        ret = picoquic_close(cnx, 0);
                    }
                    break;
                case picoquic_callback_stateless_reset:
                    LOG(INFO) << "picoquic_callback_stateless_reset";
                case picoquic_callback_close: /* Received connection close */
                    LOG(INFO) << "picoquic_callback_close";
                case picoquic_callback_application_close: /* Received application close */
                    LOG(INFO) << "picoquic_callback_application_close";
                    LOG(ERROR) << "Connection closed.";
                    /* Mark the connection as completed */
                    uint64_t app_error;
                    app_error = picoquic_get_application_error(quicClientConnection->_quic_cnx);
                    uint64_t local_error;
                    local_error = picoquic_get_local_error(quicClientConnection->_quic_cnx);
                    uint64_t remote_error;
                    remote_error = picoquic_get_remote_error(quicClientConnection->_quic_cnx);
                    quicClientConnection->disconnected(true);
                    /* Remove the application callback */
                    picoquic_set_callback(cnx, nullptr, nullptr);
                    break;
                case picoquic_callback_version_negotiation:
                    LOG(INFO) << "picoquic_callback_version_negotiation";
                    /* The client did not get the right version.
                     * TODO: some form of negotiation?
                     */
                    fprintf(stdout, "Received a version negotiation request:");
                    for (size_t byte_index = 0; byte_index + 4 <= length; byte_index += 4) {
                        uint32_t vn = 0;
                        for (int i = 0; i < 4; i++) {
                            vn <<= 8;
                            vn += bytes[byte_index + i];
                        }
                        fprintf(stdout, "%s%08x", (byte_index == 0) ? " " : ", ", vn);
                    }
                    fprintf(stdout, "\n");
                    break;
                case picoquic_callback_stream_gap:
                    LOG(INFO) << "picoquic_callback_stream_gap";
                    /* This callback is never used. */
                    break;
                case picoquic_callback_prepare_to_send:
                    LOG(INFO) << "picoquic_callback_prepare_to_send";
                    /* Active sending API */
                    if (quicStream == nullptr) {
                        /* Decidedly unexpected */
                        return -1;
                    } else {
                        std::string helloWorld("Hello World");
                        uint8_t* buffer = reinterpret_cast<uint8_t*>(&helloWorld[0]);
                        int is_fin = 1;

                        /* Needs to retrieve a pointer to the actual buffer
                         * the "bytes" parameter points to the sending context
                         */
                        buffer = picoquic_provide_stream_data_buffer(bytes, helloWorld.size(), is_fin, !is_fin);
                        memcpy(buffer, helloWorld.data(), helloWorld.size());
                        if (buffer != nullptr) {
                            LOG(INFO) << "SEND " << buffer;
                        }
                        else {
                            ret = -1;
                        }
                    }
                    break;
                case picoquic_callback_almost_ready:
                    LOG(INFO) << "picoquic_callback_almost_ready";
                    LOG(ERROR) << "Connection to the server completed, almost ready.";
                    //fprintf(stdout, "Connection to the server completed, almost ready.\n");
                    break;
                case picoquic_callback_ready:
                    LOG(INFO) << "picoquic_callback_ready";
                    /* TODO: Check that the transport parameters are what the sample expects */
                    LOG(ERROR) << "Connection to the server confirmed.";
                    //fprintf(stdout, "Connection to the server confirmed.\n");
                    break;
                default:
                    LOG(INFO) << "default";
                    /* unexpected -- just ignore. */
                    break;
            }
        }

        return ret;
    }

    int QuicClientConnection::loop_callback(picoquic_quic_t *quic, picoquic_packet_loop_cb_enum cb_mode,
                                            void *callback_ctx, void *callback_arg) {
        int ret = 0;
        auto* quicClientConnection = (QuicClientConnection*)callback_ctx;

        LOG(ERROR) << "QuicClientConnection::loop_callback(quic=" << quic << ", cb_mode=" << cb_mode << ", cb_ctx" << quicClientConnection->ToString() << ", callback_arg=" << callback_arg << ")";

        if (quicClientConnection == nullptr) {
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
                    if (quicClientConnection->disconnected()) {
                        ret = PICOQUIC_NO_ERROR_TERMINATE_PACKET_LOOP;
                    }
                    break;
                case picoquic_packet_loop_port_update:
                    LOG(INFO) << "picoquic_packet_loop_port_update";
                    break;
                default:
                    LOG(INFO) << "default";
                    ret = PICOQUIC_ERROR_UNEXPECTED_ERROR;
                    break;
            }
        }

        return ret;
    }

    std::string QuicClientConnection::ToString() {
        return "QuicClientConnection[]";
    }

    QuicClientConnection::~QuicClientConnection() {
        int ret = _packet_loop.get();

        LOG(ERROR) << "QuicClientConnection::~QuicClientConnection()";

        if (_quic != nullptr) {
            if (picoquic_save_session_tickets(_quic, _ticket_store_filename.c_str()) != 0) {
                fprintf(stderr, "Could not store the saved session tickets.\n");
            }
            if (picoquic_save_retry_tokens(_quic, _token_store_filename.c_str()) != 0) {
                fprintf(stderr, "Could not save tokens to <%s>.\n", _token_store_filename.c_str());
            }
        }

        //free_context(quicClientConnection);
    }
} // net