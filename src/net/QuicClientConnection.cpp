//
// Created by Matthias Hofstätter on 31.07.23.
//

#include <utility>

#include "QuicClientConnection.h"

#include <glog/logging.h>
#include <picoquic_utils.h>
#include <cassert>
#include <atomic>

#include "QuicStream.h"
#include "Flow.h"
#include "../packet/FlowHeader.h"
#include "SAT.h"
#include "TER.h"
#include "../backlog/Chunk.h"
#include "../backlog/TotalBacklog.h"

namespace net {
    QuicClientConnection::QuicClientConnection(std::string server_name, int port, bool is_sat, std::string ticket_store_name, std::string token_store_name) : QuicConnection(is_sat),
                                                                                                                                                              _server_name(server_name),
                                                                                                                                                              _port(port),
                                                                                                                                                              _ready(false),
                                                                                                                                                              _ticket_store_filename(std::move(ticket_store_name)),
                                                                                                                                                              _token_store_filename(std::move(token_store_name)) {
        std::lock_guard lock(_quic_mutex);

        LOG(ERROR) << "QuicClientConnection::QuicClientConnection(" << _server_name << ", " << port << ")";

        int ret = 0;

        /* Get the server's address */
        if (ret == 0) {
            int is_name = 0;

            ret = picoquic_get_server_address(_server_name.c_str(), _port, &_server_address, &is_name);
            if (ret != 0) {
                LOG(ERROR) << "Cannot get the IP address for " << _server_name << " port " << _port;
            }
            else if (is_name) {
                _sni = _server_name;
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
                ret = -1;
            }
            else {
                if (picoquic_load_retry_tokens(_quic, _token_store_filename.c_str()) != 0) {
                    LOG(ERROR) << "No token file present. Will create one as " << _token_store_filename << ".";
                }

                picoquic_set_default_congestion_algorithm(_quic, picoquic_bbr_algorithm);

                picoquic_set_key_log_file_from_env(_quic);
                picoquic_set_qlog(_quic, "./");
                picoquic_set_log_level(_quic, 1);
                // development/debugging, otherwise error 1075 IDLE_TIMEOUT, sender.c:4271 picoquic_check_idle_timer(...)
                picoquic_set_default_idle_timeout(_quic, 1000000000);
                // https://github.com/private-octopus/picoquic/blob/1e2979e8db0957c8ee798940091c4d0ef13bf8af/picoquic/picoquic.h#L1088
                picoquic_set_default_priority(_quic, 0xA);
            }
        }

        /* Initialize the callback context and create the connection context.
         * We use minimal options on the client side, keeping the transport
         * parameter values set by default for picoquic. This could be fixed later.
         */

        if (ret == 0) {
            LOG(INFO) << "Starting connection to " << _server_name << ", port " << _port;

            /* Create a client connection */
            _quic_cnx = picoquic_create_cnx(_quic, picoquic_null_connection_id, picoquic_null_connection_id,
                                            (struct sockaddr *) &_server_address, _current_time, 0, _sni.c_str(),
                                            PICOQUIC_SAMPLE_ALPN, 1);


            if (_quic_cnx == nullptr) {
                LOG(ERROR) << "Could not create connection context";
                ret = -1;
            } else {
                /* Set the client callback context */
                picoquic_set_callback(_quic_cnx, &QuicClientConnection::callback, this);
                /* Client connection parameters could be set here, before starting the connection. */
                ret = picoquic_start_client_cnx(_quic_cnx);
                if (ret < 0) {
                    LOG(ERROR) << "Could not activate connection";
                } else {
                    /* Printing out the initial CID, which is used to identify log files */
                    picoquic_connection_id_t icid = picoquic_get_initial_cnxid(_quic_cnx);
                    LOG(INFO) << "Initial connection ID: " << icid.id;
                }
            }

        }

        /* Wait for packets */
        _packet_loop = std::async(std::launch::async,[this] {
            return picoquic_packet_loop(_quic, 0, _server_address.ss_family, 0, 0, 0, &QuicClientConnection::loop_callback, this);
        });

        // wait until connected TODO
        while(_disconnected) {}
    }

    int QuicClientConnection::callback(picoquic_cnx_t *cnx, uint64_t stream_id, uint8_t *bytes, size_t length,
                                       picoquic_call_back_event_t fin_or_event, void *callback_ctx,
                                       void *v_stream_ctx) {
        int ret = 0;
        auto* connection = (QuicClientConnection*)callback_ctx;
        auto* stream = (QuicStream*)v_stream_ctx;

        std::string connectionName = (connection->_is_sat) ? "SAT" : "TER";
        //LOG(INFO) << connectionName <<  "::callback(cnx=" << cnx << ", stream_id=" << stream_id << ", bytes=[skip], length=" << length << ", fin_or_event=" << fin_or_event << ", callback_ctx=" << callback_ctx << ", v_stream_ctv=" << v_stream_ctx << ")";

        if (connection == nullptr) {
            /* This should never happen, because the callback context for the client is initialized
             * when creating the client connection. */
            return -1;
        }

        if (ret == 0) {
            switch (fin_or_event) {
                case picoquic_callback_stream_data:
                    //LOG(INFO) << "picoquic_callback_stream_data";
                case picoquic_callback_stream_fin: {
                    //LOG(INFO) << "picoquic_callback_stream_fin";

                    // create stream if not exists
                    if (stream == nullptr) {
                        // create rx stream
                        assert(connection->_streams[stream_id - 1] != nullptr);
                        assert(length == sizeof(packet::FlowHeader));

                        // the other _flow is created as soon as a packet is received on the other connection
                        stream = connection->create_stream(stream_id, connection->_streams[stream_id -1]->flow());

                        break;
                    }

                    assert(stream != nullptr);

                    // skip gap
                    if (length < sizeof(backlog::ChunkHeader)) {
                        break;
                    }

                    // copy bytes to buffer
                    memcpy(stream->_recv_buffer + stream->_recv_buffer_size, bytes, length);
                    stream->_recv_buffer_size += length;

                    if (stream->_recv_buffer_size < sizeof(backlog::ChunkHeader)) {
                        break; // need more data
                    }

                    // read chunk header
                    backlog::ChunkHeader chunk_header;
                    memcpy(&chunk_header, stream->_recv_buffer, sizeof(backlog::ChunkHeader));

                    if (stream->_recv_buffer_size < sizeof(backlog::ChunkHeader) + chunk_header.size) {
                        break; // need more data
                    }

                    // read chunk data
                    auto *chunk = new backlog::Chunk;
                    chunk->header = chunk_header;
                    chunk->data = static_cast<unsigned char *>(malloc(chunk->header.size));
                    memcpy(chunk->data, stream->_recv_buffer + sizeof(backlog::ChunkHeader), chunk->header.size);

                    // write chunk to tx queue
                    stream->flow()->tx().insert(chunk);

                    // reset recv buffer if packet complete
                    stream->_recv_buffer_size = 0;

                    LOG(INFO) << stream->to_string() << " -> " << chunk_header.to_string();

                    break;
                }
                case picoquic_callback_prepare_to_send: {
                        //LOG(INFO) << "picoquic_callback_prepare_to_send";
                        /* Active sending API */
                        if (stream == nullptr) {
                            /* Decidedly unexpected */
                            return -1;
                        } else {
                            //LOG(INFO) << "BACKLOG: totalBacklog=" << backlog::total_backlog << ", backlog=" << stream->flow()->backlog;

                            // skip if wrong connection
                            if (connection->_is_sat) {
                                if (!stream->_flow->_use_satellite.load()) {
                                    if (backlog::total_backlog < 74219 || stream->flow()->backlog <= 2000) {
                                        LOG(INFO) << "SWITCH TO TER";
                                        //picoquic_provide_stream_data_buffer(bytes, 0, 0, 0);
                                        connection->mark_active_stream(stream_id, 0, stream);
                                        TER->_streams[stream_id]->mark_active_stream(1);
                                        break;
                                    }
                                }

                                stream->flow()->_use_satellite.store(true);
                            } else {
                                if (stream->flow()->use_satellite() || (backlog::total_backlog >= 75000 && stream->flow()->backlog > 2000)) {
                                    LOG(INFO) << "SWITCH TO SAT";
                                    //picoquic_provide_stream_data_buffer(bytes, 0, 0, 0);
                                    connection->mark_active_stream(stream_id, 0, stream);
                                    SAT->_streams[stream_id]->mark_active_stream(1);
                                    break;
                                }
                            }

                            uint8_t *buffer;
                            // gap is not supported by picoquic yet
                            if (length < sizeof(backlog::ChunkHeader)) {
                                //picoquic_provide_stream_data_buffer(bytes, length, 0, 1);
                                break;
                            }

                            // get chunk
                            backlog::Chunk *chunk = stream->flow()->backlog.next(length - sizeof(backlog::ChunkHeader));
                            if (chunk == nullptr) {
                                picoquic_provide_stream_data_buffer(bytes, 0, 0, 1); // active = 0
                                //connection->mark_active_stream(stream_id, 0, stream);
                                break;
                            }

                            // send Chunk
                            buffer = picoquic_provide_stream_data_buffer(bytes, sizeof(backlog::ChunkHeader) +
                                                                                chunk->header.size, (chunk->header.size == 0) ? 1 : 0, (chunk->header.size == 0) ? 0 : 1);
                            if (buffer != nullptr) {
                                // write header
                                backlog::ChunkHeader chunkHeader;
                                chunkHeader.offset = chunk->header.offset;
                                chunkHeader.size = chunk->header.size;

                                memcpy(buffer, &chunkHeader, sizeof(backlog::ChunkHeader));

                                // write data
                                memcpy(buffer + sizeof(backlog::ChunkHeader), chunk->data, chunk->header.size);

                                LOG(INFO) << chunkHeader.to_string() << " -> " << stream->to_string();
                            }
                        }

                        break;
                    }
                case picoquic_callback_stop_sending: /* Should not happen, treated as reset */
                    LOG(INFO) << "picoquic_callback_stop_sending";
                    /* Fall through */
                case picoquic_callback_stream_reset: /* Server reset stream #x */
                    LOG(INFO) << "picoquic_callback_stream_reset";
                    break;
                case picoquic_callback_stateless_reset: /* Received an error message */
                    LOG(INFO) << "picoquic_callback_stateless_reset";
                case picoquic_callback_close: /* Received connection close */
                    LOG(INFO) << "picoquic_callback_close";
                case picoquic_callback_application_close: /* Received application close */
                    LOG(INFO) << "picoquic_callback_application_close";
                    LOG(ERROR) << "Connection closed.";
                    /* Mark the connection as completed */
                    uint64_t app_error;
                    app_error = picoquic_get_application_error(connection->_quic_cnx);
                    uint64_t local_error;
                    local_error = picoquic_get_local_error(connection->_quic_cnx);
                    uint64_t remote_error;
                    remote_error = picoquic_get_remote_error(connection->_quic_cnx);
                    LOG(INFO) << "appError=" << app_error << ", localErro=" << local_error << ", remote_error=" << remote_error;
                    /* Remove the application callback */
                    //delete connection; // TODO
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
                case picoquic_callback_almost_ready:
                    LOG(INFO) << "picoquic_callback_almost_ready";
                    LOG(ERROR) << "Connection to the server completed, almost ready.";
                    //fprintf(stdout, "Connection to the server completed, almost ready.\n");
                    break;
                case picoquic_callback_ready:
                    LOG(INFO) << "picoquic_callback_ready";
                    /* TODO: Check that the transport parameters are what the sample expects */

                    // set _disconnected
                    connection->_disconnected = false;

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
        auto* connection = (QuicClientConnection*)callback_ctx;

        if (connection == nullptr) {
            ret = PICOQUIC_ERROR_UNEXPECTED_ERROR;
        }
        else {
            switch (cb_mode) {
                case picoquic_packet_loop_ready: {
                    LOG(ERROR) << "QuicClientConnection::loop_callback(quic=" << quic << ", cb_mode=picoquic_packet_loop_ready, cb_ctx=" << connection->to_string() << ", callback_arg=" << callback_arg << ")";
                    LOG(INFO) << "Waiting for packets.";

                    // set do_time_check
                    auto *options = static_cast<picoquic_packet_loop_options_t *>(callback_arg);
                    int enable = 1;
                    options->do_time_check = enable;
                    break;
                }
                case picoquic_packet_loop_after_receive: {
                    //LOG(ERROR) << "QuicClientConnection::loop_callback(quic=" << quic << ", cb_mode=picoquic_packet_loop_after_receive, cb_ctx=" << connection->to_string() << ", callback_arg=" << callback_arg << ")";
                    break;
                }
                case picoquic_packet_loop_after_send: {
                    //LOG(ERROR) << "QuicClientConnection::loop_callback(quic=" << quic << ", cb_mode=picoquic_packet_loop_after_send, cb_ctx=" << connection->to_string() << ", callback_arg=" << callback_arg << ")";
                    /*if (*((size_t*)callback_arg) > 0 && !connection->_disconnected) {
                        total_backlog -= *((size_t *) callback_arg);
                        LOG(INFO) << "TOTALBACKLOG totalbacklog=" << total_backlog;
                    }*/
                    break;
                }
                case picoquic_packet_loop_port_update: {
                    LOG(ERROR) << "QuicClientConnection::loop_callback(quic=" << quic << ", cb_mode=picoquic_packet_loop_port_update, cb_ctx=" << connection->to_string() << ", callback_arg=" << callback_arg << ")";
                    break;
                }
                case picoquic_packet_loop_time_check: {
                    auto *arg = static_cast<packet_loop_time_check_arg_t *>(callback_arg);
                    if (arg->delta_t > 2) {
                        arg->delta_t = 2;
                    }
                    break;
                }
                default: {
                    //LOG(ERROR) << "QuicClientConnection::loop_callback(quic=" << quic << ", cb_mode=default, cb_ctx=" << connection->to_string() << ", callback_arg=" << callback_arg << ")";
                    ret = PICOQUIC_ERROR_UNEXPECTED_ERROR;
                    break;
                }
            }
        }

        return ret;
    }

    std::string QuicClientConnection::to_string() {
        std::stringstream s;
        s << "QuicClientConnection[quic=" << _quic << ", cnx=" << _quic_cnx << ", server_name=" << _server_name << ", port=" << std::to_string(_port) << "]";
        return s.str();
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