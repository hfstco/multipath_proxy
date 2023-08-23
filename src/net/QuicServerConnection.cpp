//
// Created by Matthias Hofstätter on 31.07.23.
//

#include "QuicServerConnection.h"

#include <glog/logging.h>
#include <picoquic_packet_loop.h>
#include <cassert>

#include "QuicConnection.h"
#include "QuicStream.h"
#include "SockAddr.h"
#include "TcpConnection.h"
#include "Flow.h"
#include "../packet/FlowHeader.h"
#include "TER.h"
#include "SAT.h"
#include "../backlog/TotalBacklog.h"
#include "../backlog/Chunk.h"

namespace net {
    QuicServerConnection::QuicServerConnection(int port, bool is_sat) : QuicConnection(is_sat), _port(port) {
        std::lock_guard lock(_quic_mutex);

        LOG(ERROR) << "QuicServerConnection::QuicServerConnection(" << port << ")";

        // create quic
        _quic = picoquic_create(100, "/root/cert.pem", "/root/key.pem", nullptr, PICOQUIC_SAMPLE_ALPN, &QuicServerConnection::callback,
                                this, nullptr, nullptr, nullptr, _current_time, nullptr, nullptr, nullptr,
                                0);

        // configure quic
        picoquic_set_cookie_mode(_quic, 2);
        picoquic_set_default_congestion_algorithm(_quic, picoquic_bbr_algorithm);
        picoquic_set_qlog(_quic, "./");
        picoquic_set_log_level(_quic, 1);
        picoquic_set_key_log_file_from_env(_quic);
        // for debug
        picoquic_set_default_idle_timeout(_quic, 1000000000);
        // https://github.com/private-octopus/picoquic/blob/1e2979e8db0957c8ee798940091c4d0ef13bf8af/picoquic/picoquic.h#L1088
        picoquic_set_default_priority(_quic, 0xA);

        // start quic loop
        _packet_loop = std::async([this]{
            return picoquic_packet_loop(_quic, _port, 0, 0, 0, 0, &QuicServerConnection::loop_callback, this);
        });
    }

    int QuicServerConnection::loop_callback(picoquic_quic_t *quic, picoquic_packet_loop_cb_enum cb_mode,
                             void *callback_ctx, void *callback_arg) {
        int ret = 0;
        auto* connection = (QuicServerConnection*)callback_ctx;

        if (connection == NULL) {
            ret = PICOQUIC_ERROR_UNEXPECTED_ERROR;
        }
        else {
            switch (cb_mode) {
                case picoquic_packet_loop_ready: {
                    LOG(INFO) << "QuicServerConnection::loop_callback(quic=" << quic << ", cb_mode=picoquic_packet_loop_ready, callback_ctx=" << callback_ctx << ", callback_arg=" << callback_arg << ")";
                    // set do_time_check
                    auto *options = static_cast<picoquic_packet_loop_options_t *>(callback_arg);
                    int enable = 1;
                    options->do_time_check = enable;
                    break;
                    }
                case picoquic_packet_loop_after_receive: {
                    //LOG(INFO) << "QuicServerConnection::loop_callback(quic=" << quic << ", cb_mode=picoquic_packet_loop_after_receive, callback_ctx=" << callback_ctx << ", callback_arg=" << callback_arg << ")";
                    break;
                    }
                case picoquic_packet_loop_after_send: {
                    //LOG(INFO) << "QuicServerConnection::loop_callback(quic=" << quic << ", cb_mode=picoquic_packet_loop_after_send, callback_ctx=" << callback_ctx << ", callback_arg=" << callback_arg << ")";
                    /*if (*((size_t*)callback_arg) > 0 && !connection->_disconnected) {
                        total_backlog -= *((size_t *) callback_arg);
                        LOG(INFO) << "TOTALBACKLOG totalbacklog=" << total_backlog;
                    }*/
                    break;
                    }
                case picoquic_packet_loop_port_update: {
                    LOG(INFO) << "QuicServerConnection::loop_callback(quic=" << quic << ", cb_mode=picoquic_packet_loop_port_update, callback_ctx=" << callback_ctx << ", callback_arg=" << callback_arg << ")";
                    break;
                }
                case picoquic_packet_loop_time_check: {
                    //LOG(INFO) << "QuicServerConnection::loop_callback(quic=" << quic << ", cb_mode=picoquic_packet_loop_time_check, callback_ctx=" << callback_ctx << ", callback_arg=" << callback_arg << ")";
                    auto *arg = static_cast<packet_loop_time_check_arg_t *>(callback_arg);
                    if (arg->delta_t > 2) {
                        arg->delta_t = 2;
                    }
                    break;
                }
                default: {
                    LOG(INFO) << "QuicServerConnection::loop_callback(quic=" << quic << ", cb_mode=default, callback_ctx=" << callback_ctx << ", callback_arg=" << callback_arg << ")";
                    ret = PICOQUIC_ERROR_UNEXPECTED_ERROR;
                    break;
                }
            }
        }
        return ret;
    }

    int QuicServerConnection::callback(picoquic_cnx_t* cnx,
                                       uint64_t stream_id, uint8_t* bytes, size_t length,
                                       picoquic_call_back_event_t fin_or_event, void* callback_ctx, void* v_stream_ctx) {
        int ret = 0;
        auto* connection = (QuicServerConnection*)callback_ctx;
        auto* stream = (QuicStream*)v_stream_ctx;

        std::string connectionName = (connection->_is_sat) ? "SAT" : "TER";
        //LOG(ERROR) << connectionName << "::callback(cnx=" << cnx << ", stream_id=" << stream_id << ", bytes=[skip], length=" << length << ", fin_or_event=" << fin_or_event << ", callback_ctx=" << callback_ctx << ", v_stream_ctv=" << v_stream_ctx << ")";


        // TODO init somewhere else
        if (connection->_quic_cnx == nullptr) {
            connection->_quic_cnx = cnx;
        }

        switch (fin_or_event) {
            case picoquic_callback_stream_data:
                //LOG(INFO) << "picoquic_callback_stream_data";
            case picoquic_callback_stream_fin: {
                //LOG(INFO) << "picoquic_callback_stream_fin";

                // create stream if not exists
                if (stream == nullptr) {
                    assert(length == sizeof(packet::FlowHeader)); // check if first packet is flow header

                    packet::FlowHeader flowHeader;
                    memcpy(&flowHeader, bytes, sizeof(packet::FlowHeader));

                    {
                        std::lock_guard lock(connection->_quic_mutex);

                        Flow *flow = nullptr;
                        if (!TER->stream_exists(stream_id + 1) && !SAT->stream_exists(
                                stream_id + 1)) { // first connection create back streams if they don't exist
                            // create flow
                            net::ipv4::SockAddr_In source = net::ipv4::SockAddr_In(flowHeader.source_ip,
                                                                                   flowHeader.source_port);
                            net::ipv4::SockAddr_In destination = net::ipv4::SockAddr_In(flowHeader.destination_ip,
                                                                                        flowHeader.destination_port);
                            net::ipv4::TcpConnection *tcp_connection = net::ipv4::TcpConnection::make(destination);
                            flow = net::Flow::make(source, destination, tcp_connection, stream_id + 1);
                            // TODO try, error -> reset
                        } else { // second connection get flow from other connection
                            flow = (connection->_is_sat) ? TER->_streams[stream_id + 1]->flow()
                                                         : SAT->_streams[stream_id + 1]->flow();
                        }

                        // create stream for current connection
                        stream = connection->create_stream(stream_id, flow);
                        //stream->set_app_stream_ctx();
                    }

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
                    /* This should never happen */
                } else {
                    // wait for Flow
                    assert (stream->flow() != nullptr);

                    //LOG(INFO) << "BACKLOG: totalBacklog=" << backlog::total_backlog << ", backlog=" << stream->flow()->backlog;

                    // switch connection
                    if (connection->_is_sat) {
                        if (!stream->flow()->_use_satellite.load()) {
                            if (backlog::TotalBacklog::size() < 74219 ||
                                stream->flow()->backlog <= 2000) {
                                LOG(INFO) << "SWITCH TO TER";
                                //picoquic_provide_stream_data_buffer(bytes, 0, 0, 0);
                                connection->mark_active_stream(stream_id, 0, stream);
                                TER->_streams[stream_id]->mark_active_stream(1);
                                break;
                            }
                        }

                        stream->flow()->_use_satellite.store(true);
                    } else {
                        if (stream->flow()->use_satellite() ||
                            (backlog::TotalBacklog::size() >= 75000 && stream->flow()->backlog > 2000)) {
                            LOG(INFO) << "SWITCH TO SAT";
                            //picoquic_provide_stream_data_buffer(bytes, 0, 0, 0);
                            stream->mark_active_stream(0);
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
                        picoquic_provide_stream_data_buffer(bytes, 0, 0, 1);
                        //stream->mark_active_stream(0);
                        break;
                    }

                    //assert(sizeof(packet::ChunkHeader) + chunk->size == length);

                    // send Chunk
                    buffer = picoquic_provide_stream_data_buffer(bytes, sizeof(backlog::ChunkHeader) + chunk->header.size, (chunk->header.size == 0) ? 1 : 0, (chunk->header.size == 0) ? 0 : 1);
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
            case picoquic_callback_stream_reset: /* Client reset stream #x */
                LOG(INFO) << "picoquic_callback_stream_reset";
            case picoquic_callback_stop_sending: /* Client asks server to reset stream #x */
                LOG(INFO) << "picoquic_callback_stop_sending";
                break;
            case picoquic_callback_stateless_reset: /* Received an error message */
                LOG(INFO) << "picoquic_callback_stateless_reset";
            case picoquic_callback_close: /* Received connection close */
                LOG(INFO) << "picoquic_callback_close";
            case picoquic_callback_application_close: /* Received application close */
                LOG(INFO) << "picoquic_callback_application_close";
                LOG(INFO) << "Connection closed.";
                uint64_t app_error;
                app_error = picoquic_get_application_error(connection->_quic_cnx);
                uint64_t local_error;
                local_error = picoquic_get_local_error(connection->_quic_cnx);
                uint64_t remote_error;
                remote_error = picoquic_get_remote_error(connection->_quic_cnx);
                LOG(INFO) << "appError=" << app_error << ", localErro=" << local_error << ", remote_error=" << remote_error;
                /* Delete the server application context */
                //delete connection; // TODO
                break;
            case picoquic_callback_version_negotiation:
                LOG(INFO) << "picoquic_callback_version_negotiation";
                /* The server should never receive a version negotiation response */
                break;
            case picoquic_callback_stream_gap:
                LOG(INFO) << "picoquic_callback_stream_gap";
                /* This callback is never used. */
                break;
            case picoquic_callback_almost_ready:
                LOG(INFO) << "picoquic_callback_almost_ready";
                break;
            case picoquic_callback_ready:
                LOG(INFO) << "picoquic_callback_ready";
                /* Check that the transport parameters are what the sample expects */

                // set _disconnected
                connection->_disconnected = false;

                break;
            default:
                LOG(INFO) << "default";
                /* unexpected */
                break;
        }

        return ret;
    }

    std::string QuicServerConnection::to_string() {
        std::stringstream s;
        s << "QuicServerConnection[quic=" << _quic << ", cnx=" << _quic_cnx << ", port=" << std::to_string(_port) << ", isSat=" << std::to_string(_is_sat) << "]";
        return s.str();
    }

    QuicServerConnection::~QuicServerConnection() {
        int ret = _packet_loop.get();

        LOG(ERROR) << "QuicServerConnection::~QuicServerConnection()";
    }

}
