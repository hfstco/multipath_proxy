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
#include "../packet/ChunkHeader.h"

namespace net {
    QuicServerConnection::QuicServerConnection(int port, bool is_sat) : QuicConnection(is_sat), _port(port) {
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

        // start quic loop
        _packet_loop = std::async([this]{
            return picoquic_packet_loop(_quic, _port, 0, 0, 0, 0, &QuicServerConnection::loop_callback, this);
        });
    }

    QuicStream *QuicServerConnection::CreateNewStream(bool bidirectional, net::Flow *flow) {
        std::lock_guard lock(_streams_mutex);

        LOG(INFO) << ToString() << ".CreateStream()";

        uint64_t stream_id = -1;
        for (uint64_t i = bidirectional ? 1 : 3; i < _streams.size(); i += 4) {
            if (_streams[i] == nullptr && _streams[i - 1] == nullptr) {
                stream_id = i;
                break;
            }
        }

        if (stream_id == -1) {
            LOG(ERROR) << "No free streams.";
            return nullptr;
        }

        auto *quicStream = new QuicStream(_quic_cnx, stream_id, flow);
        _streams[stream_id] = quicStream;
        return quicStream;
    }

    int QuicServerConnection::loop_callback(picoquic_quic_t *quic, picoquic_packet_loop_cb_enum cb_mode,
                             void *callback_ctx, void *callback_arg) {
        int ret = 0;
        auto* quicServerConnection = (QuicServerConnection*)callback_ctx;

        if (quicServerConnection == NULL) {
            ret = PICOQUIC_ERROR_UNEXPECTED_ERROR;
        }
        else {
            switch (cb_mode) {
                case picoquic_packet_loop_ready: {
                    //LOG(INFO) << "QuicServerConnection::loop_callback(quic=" << quic << ", cb_mode=picoquic_packet_loop_ready, callback_ctx=" << callback_ctx << ", callback_arg=" << callback_arg << ")";
                    LOG(INFO) << "Waiting for packets.";

                    //set
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
                    break;
                    }
                case picoquic_packet_loop_port_update: {
                    //LOG(INFO) << "QuicServerConnection::loop_callback(quic=" << quic << ", cb_mode=picoquic_packet_loop_port_update, callback_ctx=" << callback_ctx << ", callback_arg=" << callback_arg << ")";
                    break;
                }
                case picoquic_packet_loop_time_check: {
                    //LOG(INFO) << "QuicServerConnection::loop_callback(quic=" << quic << ", cb_mode=picoquic_packet_loop_time_check, callback_ctx=" << callback_ctx << ", callback_arg=" << callback_arg << ")";
                    auto *arg = static_cast<packet_loop_time_check_arg_t *>(callback_arg);
                    if (arg->delta_t > 100) {
                        arg->delta_t = 100;
                    }
                    break;
                }
                default: {
                    //LOG(INFO) << "QuicServerConnection::loop_callback(quic=" << quic << ", cb_mode=default, callback_ctx=" << callback_ctx << ", callback_arg=" << callback_arg << ")";
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
        auto* quicServerConnection = (QuicServerConnection*)callback_ctx;
        auto* quicStream = (QuicStream*)v_stream_ctx;

        std::string connectionName = (quicServerConnection->_is_sat) ? "SAT" : "TER";
        //LOG(ERROR) << connectionName << "::callback(cnx=" << cnx << ", stream_id=" << stream_id << ", bytes=[skip], length=" << length << ", fin_or_event=" << fin_or_event << ", callback_ctx=" << callback_ctx << ", v_stream_ctv=" << v_stream_ctx << ")";


        // TODO init somewhere else
        if (quicServerConnection->_quic_cnx == nullptr) {
            quicServerConnection->_quic_cnx = cnx;
        }

        switch (fin_or_event) {
            case picoquic_callback_stream_data:
                //LOG(INFO) << "picoquic_callback_stream_data";
            case picoquic_callback_stream_fin: {
                //LOG(INFO) << "picoquic_callback_stream_fin";

                // create stream if not exists
                if (quicStream == nullptr) {
                    // create stream
                    assert(length == sizeof(packet::FlowHeader)); // check if first packet is FlowHeader

                    packet::FlowHeader flowHeader;
                    memcpy(&flowHeader, bytes, sizeof(packet::FlowHeader));

                    LOG(INFO) << "RECV " << flowHeader.ToString();

                    net::Flow *flow;
                    if (!TER->StreamExists(stream_id + 1) && !SAT->StreamExists(stream_id + 1)) {
                        // create flow
                        net::ipv4::SockAddr_In source = net::ipv4::SockAddr_In(flowHeader.sourceIP, flowHeader.sourcePort);
                        net::ipv4::SockAddr_In destination = net::ipv4::SockAddr_In(flowHeader.destinationIP, flowHeader.destinationPort);
                        net::ipv4::TcpConnection *tcpConnection = net::ipv4::TcpConnection::make(destination);
                        flow = net::Flow::make(source, destination, tcpConnection);
                        // TODO try, error -> reset

                        // create return channels
                        auto *terQuicStream = TER->CreateStream(stream_id + 1, flow);
                        flow->_terTxStream = terQuicStream;
                        terQuicStream->MarkActiveStream();

                        auto *satQuicStream = SAT->CreateStream(stream_id + 1, flow);
                        flow->_satTxStream = satQuicStream;
                        satQuicStream->MarkActiveStream();

                        flow->MakeActiveFlow();
                    }

                    if (flow == nullptr) {
                        quicStream = quicServerConnection->CreateStream(stream_id, (quicServerConnection->_is_sat) ? TER->_streams[stream_id]->_flow : SAT->_streams[stream_id]->_flow);
                    } else {
                        quicStream = quicServerConnection->CreateStream(stream_id, flow);
                    }

                    LOG(INFO) << "RECV " << flowHeader.ToString();

                    break;
                }

                assert(quicStream != nullptr);

                assert(length < 1500 - quicStream->_rxBufferSize); // TODO rxBuffer size

                memcpy(quicStream->_rxBuffer + quicStream->_rxBufferSize, bytes, length);
                quicStream->_rxBufferSize += length;

                // get Chunk header
                if (quicStream->_rxBufferSize < sizeof(packet::ChunkHeader)) {
                    break; // need more data
                }

                packet::ChunkHeader chunkHeader;
                memcpy(&chunkHeader, quicStream->_rxBuffer, sizeof(packet::ChunkHeader));

                // get Chunk data
                if (quicStream->_rxBufferSize < sizeof(packet::ChunkHeader) + chunkHeader.size) {
                    break; // need more data
                }

                backlog::Chunk *chunk = new backlog::Chunk;
                chunk->offset = chunkHeader.offset;
                chunk->size = chunkHeader.size;
                chunk->data = static_cast<unsigned char *>(malloc(chunk->size));
                memcpy(chunk->data, quicStream->_rxBuffer + sizeof(packet::ChunkHeader), chunk->size);

                // write Chunk to tx backlog
                quicStream->flow()->tx().Insert(chunk);

                // reset rx buffer
                quicStream->_rxBufferSize = 0;

                LOG(INFO) << "RECV " << chunkHeader.ToString();

                break;
            }
            case picoquic_callback_prepare_to_send:
                //LOG(INFO) << "picoquic_callback_prepare_to_send";
                /* Active sending API */
                if (quicStream == nullptr) {
                    /* This should never happen */
                } else {
                    // wait for Flow
                    assert (quicStream->flow() != nullptr);

                    LOG(INFO) << "BACKLOG: totalBacklog=" << backlog::TotalBacklog::size() << ", backlog=" << quicStream->flow()->_rxBacklog.size();

                    // switch connection
                    if (quicServerConnection->_is_sat) {
                        if (!quicStream->_flow->_useSatellite.load()) {
                            if (backlog::TotalBacklog::size() < 74219 || quicStream->flow()->Backlog().size() <= 2000) {
                                LOG(INFO) << "SKIP " << "SAT";
                                picoquic_provide_stream_data_buffer(bytes, 0, 0, 0);
                                TER->_streams[stream_id]->MarkActiveStream();
                                break;
                            }
                        }

                        quicStream->flow()->_useSatellite.store(true);
                    } else {
                        if (quicStream->flow()->useSatellite() || (backlog::TotalBacklog::size() >= 75000 && quicStream->flow()->rx().size() > 2000)) {
                            LOG(INFO) << "SKIP " << "TER";
                            picoquic_provide_stream_data_buffer(bytes, 0, 0, 0);
                            SAT->_streams[stream_id]->MarkActiveStream();
                            break;
                        }
                    }

                    // buffer to small
                    if(length < sizeof(packet::ChunkHeader)) {
                        picoquic_provide_stream_data_buffer(bytes, 0, 0, 1);
                    }

                    // get chunk
                    backlog::Chunk *chunk = quicStream->flow()->Backlog().Next(length - sizeof(packet::ChunkHeader));
                    if (chunk == nullptr) {
                        picoquic_provide_stream_data_buffer(bytes, 0, 0, 0);
                        break;
                    }

                    //assert(sizeof(packet::ChunkHeader) + chunk->size == length);

                    // send Chunk
                    uint8_t *buffer;
                    buffer = picoquic_provide_stream_data_buffer(bytes, sizeof(packet::ChunkHeader) + chunk->size, 0, 1);
                    if (buffer != nullptr) {
                        // write header
                        packet::ChunkHeader chunkHeader;
                        chunkHeader.offset = chunk->offset;
                        chunkHeader.size = chunk->size;

                        memcpy(buffer, &chunkHeader, sizeof(packet::ChunkHeader));

                        // write data
                        memcpy(buffer + sizeof(packet::ChunkHeader), chunk->data, chunk->size);

                        LOG(INFO) << "SEND " << chunkHeader.ToString();
                    }
                }
                break;
            case picoquic_callback_stream_reset: /* Client reset stream #x */
                LOG(INFO) << "picoquic_callback_stream_reset";
            case picoquic_callback_stop_sending: /* Client asks server to reset stream #x */
                LOG(INFO) << "picoquic_callback_stop_sending";
                if (quicStream != nullptr) {
                    /* Mark stream as abandoned, close the file, etc. */
                    quicServerConnection->CloseStream(quicStream->_id);
                }
                break;
            case picoquic_callback_stateless_reset: /* Received an error message */
                LOG(INFO) << "picoquic_callback_stateless_reset";
            case picoquic_callback_close: /* Received connection close */
                LOG(INFO) << "picoquic_callback_close";
            case picoquic_callback_application_close: /* Received application close */
                LOG(INFO) << "picoquic_callback_application_close";
                LOG(INFO) << "Connection closed.";
                uint64_t app_error;
                app_error = picoquic_get_application_error(quicServerConnection->_quic_cnx);
                uint64_t local_error;
                local_error = picoquic_get_local_error(quicServerConnection->_quic_cnx);
                uint64_t remote_error;
                remote_error = picoquic_get_remote_error(quicServerConnection->_quic_cnx);
                LOG(INFO) << "appError=" << app_error << ", localErro=" << local_error << ", remoteError=" << remote_error;
                /* Delete the server application context */
                //delete quicServerConnection; // TODO
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
            case picoquic_callback_ready:
                LOG(INFO) << "picoquic_callback_ready";
                /* Check that the transport parameters are what the sample expects */
                break;
            default:
                LOG(INFO) << "default";
                /* unexpected */
                break;
        }

        return ret;
    }

    std::string QuicServerConnection::ToString() {
        std::stringstream s;
        s << "QuicServerConnection[quic=" << _quic << ", cnx=" << _quic_cnx << ", port=" << std::to_string(_port) << ", isSat=" << std::to_string(_is_sat) << "]";
        return s.str();
    }

    QuicServerConnection::~QuicServerConnection() {
        int ret = _packet_loop.get();

        LOG(ERROR) << "QuicServerConnection::~QuicServerConnection()";
    }

}
