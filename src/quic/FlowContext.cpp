//
// Created by Matthias Hofstätter on 26.08.23.
//

#include <cassert>
#include "FlowContext.h"
#include "glog/logging.h"

#include "Quic.h"
#include "../flow/Flow.h"
#include "Path.h"
#include "../backlog/Chunk.h"
#include "../net/TcpConnection.h"

namespace quic {

    FlowContext::FlowContext(Quic *quic, picoquic_cnx_t *cnx) : Context(quic, cnx) {
        // set callback
        set_callback(stream_data_cb, this);
    }

    flow::Flow *FlowContext::new_flow(net::ipv4::SockAddr_In source, net::ipv4::SockAddr_In destination,
                                      net::ipv4::TcpConnection *tcp_connection) {
        auto* flow = new_stream<flow::Flow, quic::FlowContext, net::ipv4::SockAddr_In, net::ipv4::SockAddr_In, net::ipv4::TcpConnection *>(0, source, destination, tcp_connection);

        // send flow header
        auto flowHeader = flow::FlowHeader(source.sin_addr, destination.sin_addr, source.sin_port, destination.sin_port);
        add_to_stream(flow->_stream_id, (uint8_t *)&flowHeader, sizeof(flow::FlowHeader), 0); // TODO _stream_id

        return flow;
    }

    flow::Flow *FlowContext::new_flow(uint64_t stream_id, net::ipv4::SockAddr_In source, net::ipv4::SockAddr_In destination,
                          net::ipv4::TcpConnection *tcp_connection) {
        return new_stream<flow::Flow, quic::FlowContext, net::ipv4::SockAddr_In, net::ipv4::SockAddr_In, net::ipv4::TcpConnection *>(stream_id, source, destination, tcp_connection);;
    }

    int FlowContext::stream_data_cb(picoquic_cnx_t *cnx, uint64_t stream_id, uint8_t *bytes, size_t length,
                                    picoquic_call_back_event_t fin_or_event, void *callback_ctx, void *v_stream_ctx) {
        int ret = 0;

        auto* context = (FlowContext *)callback_ctx;

        switch (fin_or_event) {
            case picoquic_callback_stream_data: {
                // get or create flow
                auto *flow = (flow::Flow *)v_stream_ctx;
                if (flow == nullptr) {
                    assert(length == sizeof(flow::FlowHeader));
                    // get flow header
                    flow::FlowHeader header;
                    memcpy(&header, bytes, sizeof(flow::FlowHeader));

                    // connect
                    net::ipv4::TcpConnection* tcp_connection = net::ipv4::TcpConnection::make(net::ipv4::SockAddr_In(header.destination_ip, header.destination_port));

                    // create new stream
                    flow = context->new_flow(stream_id, net::ipv4::SockAddr_In(header.source_ip, header.source_port), net::ipv4::SockAddr_In(header.destination_ip, header.destination_port), tcp_connection);

                    return 0;
                }

                //VLOG(3) << flow->to_string() << " picoquic_callback_stream_data";

                // write bytes into rx buffer
                memcpy(flow->_rx_buffer.data + flow->_rx_buffer.size, bytes, length);
                flow->_rx_buffer.size += length;

                // read header
                if (flow->_rx_buffer.size < sizeof(backlog::ChunkHeader)) {
                    // need more data
                    break;
                }

                backlog::ChunkHeader header;
                memcpy(&header, flow->_rx_buffer.data, sizeof(backlog::ChunkHeader));

                // read data
                if (flow->_rx_buffer.size < sizeof(backlog::ChunkHeader) + header.size) {
                    // need more data
                    break;
                }

                auto *chunk = new backlog::Chunk();
                chunk->header = header;
                chunk->data = (uint8_t*)malloc(chunk->header.size);
                memcpy(chunk->data, flow->_rx_buffer.data + sizeof(backlog::ChunkHeader), chunk->header.size);

                // insert chunk
                flow->_tx_backlog.insert(chunk);

                // reset rx buffer
                flow->_rx_buffer.size = 0;

                //flow->_connection->Send(bytes, length, 0);

                break;
            }
            case picoquic_callback_stream_fin: {
                VLOG(3) << context->to_string() << " picoquic_callback_stream_fin";
                break;
            }
            case picoquic_callback_stream_reset: {
                VLOG(3) << context->to_string() << " picoquic_callback_stream_reset";
                break;
            }
            case picoquic_callback_stop_sending: {
                VLOG(3) << context->to_string() << " picoquic_callback_stop_sending";
                break;
            }
            case picoquic_callback_stateless_reset: {
                VLOG(3) << context->to_string() << " picoquic_callback_stateless_reset";
                break;
            }
            case picoquic_callback_close: {
                VLOG(3) << context->to_string() << " picoquic_callback_close";
                break;
            }
            case picoquic_callback_application_close: {
                VLOG(3) << context->to_string() << " picoquic_callback_application_close";
                break;
            }
            case picoquic_callback_stream_gap: {
                VLOG(3) << context->to_string() << " picoquic_callback_stream_gap";
                break;
            }
            case picoquic_callback_prepare_to_send: {
                // get or create flow
                auto *flow = (flow::Flow *)v_stream_ctx;
                if (flow == nullptr) {
                    assert(length == sizeof(flow::FlowHeader));
                    // get flow header
                    flow::FlowHeader header;
                    memcpy(&header, bytes, sizeof(flow::FlowHeader));

                    // connect
                    net::ipv4::TcpConnection* tcp_connection = net::ipv4::TcpConnection::make(net::ipv4::SockAddr_In(header.destination_ip, header.destination_port));

                    // create new stream
                    flow = context->new_flow(stream_id, net::ipv4::SockAddr_In(header.source_ip, header.source_port), net::ipv4::SockAddr_In(header.destination_ip, header.destination_port), tcp_connection);
                    // set path affinity to default path
                    flow->set_stream_path_affinity(1);

                    return 0;
                }

                //VLOG(3) << flow->to_string() << " picoquic_callback_prepare_to_send";

                assert(length > sizeof(backlog::ChunkHeader));

                // get next chunk
                backlog::Chunk *chunk = flow->backlog.next(length - sizeof(backlog::ChunkHeader));

                if (chunk == nullptr) {
                    // no data available, mark flow/stream inactive
                    bool expected = true;
                    if (flow->_active.compare_exchange_strong(expected, false)) {
                        LOG(INFO) << "STREAM ACTIVE CHANGED: FALSE";
                        picoquic_provide_stream_data_buffer(bytes, 0, 0, 0);
                    }
                    break;
                }

                // send chunk
                uint8_t *buffer = picoquic_provide_stream_data_buffer(bytes, sizeof(backlog::ChunkHeader) + chunk->header.size, 0, 1);
                if (buffer != nullptr) {
                    // write chunk header
                    memcpy(buffer, &chunk->header, sizeof(backlog::ChunkHeader));
                    // write chunk data
                    memcpy(buffer + sizeof(backlog::ChunkHeader), chunk->data, chunk->header.size);
                }

                break;
            }
            case picoquic_callback_almost_ready: {
                VLOG(3) << context->to_string() << " picoquic_callback_almost_ready";
                break;
            }
            case picoquic_callback_ready: {
                VLOG(3) << context->to_string() << " picoquic_callback_ready";

                if (context->is_client()) {
                    // probe new path
                    struct sockaddr_storage addr_from;
                    int addr_from_is_name = 0;
                    struct sockaddr_storage addr_to;
                    int addr_to_is_name = 0;

                    picoquic_get_server_address("172.30.21.3", 6000, &addr_from, &addr_from_is_name); // remote_addr
                    picoquic_get_server_address("172.30.20.2", 0, &addr_to, &addr_to_is_name); // local_addr

                    //ret = context->probe_new_path_ex((struct sockaddr *) &addr_from, /*(struct sockaddr *) &addr_to*/nullptr, 0, picoquic_current_time(), 0);

                    // subscribe to quality update
                }

                // add default (stream_id=0) path and subscribe to path quality updates
                auto* path = context->new_path(0);
                ret = path->set_app_path_ctx(path); // TODO move to context->new_path()?
                //ret = path->subscribe_to_quality_update_per_path(50000, 15000);

                break;
            }
            case picoquic_callback_datagram: {
                VLOG(3) << context->to_string() << " picoquic_callback_datagram";
                break;
            }
            case picoquic_callback_version_negotiation: {
                VLOG(3) << context->to_string() << " picoquic_callback_version_negotiation";
                break;
            }
            case picoquic_callback_request_alpn_list: {
                VLOG(3) << context->to_string() << " picoquic_callback_request_alpn_list";
                break;
            }
            case picoquic_callback_set_alpn: {
                VLOG(3) << context->to_string() << " picoquic_callback_set_alpn";
                break;
            }
            case picoquic_callback_pacing_changed: {
                VLOG(3) << context->to_string() << " picoquic_callback_pacing_changed";
                break;
            }
            case picoquic_callback_prepare_datagram: {
                VLOG(3) << context->to_string() << " picoquic_callback_prepare_datagram";
                break;
            }
            case picoquic_callback_datagram_acked: {
                VLOG(3) << context->to_string() << " picoquic_callback_datagram_acked";
                break;
            }
            case picoquic_callback_datagram_lost: {
                VLOG(3) << context->to_string() << " picoquic_callback_datagram_lost";
                break;
            }
            case picoquic_callback_datagram_spurious: {
                VLOG(3) << context->to_string() << " picoquic_callback_datagram_spurious";
                break;
            }
            case picoquic_callback_path_available: {
                // TODO minimize, stream should not exist
                /*uint64_t unique_path_id = stream_id; // stream_id = unique_path_id

                auto *path = (quic::Path *)v_stream_ctx;
                if (path != nullptr) {
                    // create new stream
                    path = context->new_path(unique_path_id);
                }*/

                assert(v_stream_ctx == nullptr);
                auto* path = context->new_path(stream_id);
                ret = path->set_app_path_ctx(path); // TODO move to context->new_path()?
                //ret = path->subscribe_to_quality_update_per_path(500000, 50000);

                VLOG(3) << path->to_string() << " picoquic_callback_path_available";

                break;
            }
            case picoquic_callback_path_suspended: {
                auto *path = (quic::Path *)v_stream_ctx;
                if (path == nullptr) {
                    // create new stream
                    path = context->new_path(stream_id);
                    path->set_app_path_ctx(path); // TODO move to context->new_path()?
                }

                VLOG(3) << path->to_string() << " picoquic_callback_path_suspended";

                // TODO implement, ?

                break;
            }
            case picoquic_callback_path_deleted: {
                // TODO minimize if stream does not exists
                /*uint64_t unique_path_id = stream_id; // stream_id = unique_path_id

                auto *path = (quic::Path *)v_stream_ctx;
                if (path != nullptr) {
                    // create new stream
                    path = context->new_path(unique_path_id);
                }*/

                // TODO minimize, delete_path(stream_id) only
                auto *path = (quic::Path *)v_stream_ctx;
                if (path == nullptr) {
                    // create new stream
                    path = context->new_path(stream_id);
                    ret = path->set_app_path_ctx(path); // TODO move to context->new_path()?
                }

                VLOG(3) << path->to_string() << " picoquic_callback_path_deleted";

                context->delete_path(stream_id);

                break;
            }
            case picoquic_callback_path_quality_changed: {
                auto *path = (quic::Path *)v_stream_ctx;
                if (path == nullptr) {
                    // create new stream
                    path = context->new_path(stream_id);
                    ret = path->set_app_path_ctx(path); // TODO move to context->new_path()?
                }

                VLOG(3) << path->to_string() << " picoquic_callback_path_quality_changed";

                ret = path->get_path_quality(&path->_quality);

                break;
            }
        }

        return ret;
    }
} // quic