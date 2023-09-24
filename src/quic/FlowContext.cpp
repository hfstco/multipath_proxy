//
// Created by Matthias Hofstätter on 26.08.23.
//

#include <cassert>
#include "FlowContext.h"
#include "glog/logging.h"

#include "Quic.h"
#include "Path.h"
#include "../backlog/Chunk.h"
#include "../net/TcpConnection.h"
#include "../backlog/TotalBacklog.h"
#include "../flow/Flow.h"
#include "../threadpool/Threadpool.h"

namespace quic {

    FlowContext::FlowContext(Quic *quic, picoquic_cnx_t *cnx) : Context(quic, cnx) {
        // set callback
        set_callback(stream_data_cb, this);
    }

    // local
    flow::Flow *FlowContext::new_flow(net::ipv4::TcpConnection *connection) {
        auto* flow = new_stream<flow::Flow, quic::FlowContext, net::ipv4::TcpConnection *>(0, connection);

        // get destination address
        net::ipv4::SockAddr_In destination = connection->GetSockName();

        // TODO move to prepare_to_send?
        // send flow header
        auto flowHeader = flow::FlowHeader(destination.sin_addr, destination.sin_port);
        add_to_stream(flow->_stream_id, (uint8_t *)&flowHeader, sizeof(flow::FlowHeader), 0); // TODO _stream_id

        THREADPOOL.push_task(&flow::Flow::recv_from_connection, flow);
        THREADPOOL.push_task(&flow::Flow::send_to_connection, flow);

        return flow;
    }

    // remote
    flow::Flow *FlowContext::new_flow(uint64_t stream_id, net::ipv4::TcpConnection *connection) {
        auto* flow = new_stream<flow::Flow, quic::FlowContext, net::ipv4::TcpConnection *>(stream_id, connection);

        THREADPOOL.push_task(&flow::Flow::recv_from_connection, flow);
        THREADPOOL.push_task(&flow::Flow::send_to_connection, flow);

        return flow;
    }

    void FlowContext::delete_flow(uint64_t stream_id) {
        delete_stream(stream_id);
    }

    int FlowContext::stream_data_cb(picoquic_cnx_t *cnx, uint64_t stream_id, uint8_t *bytes, size_t length,
                                    picoquic_call_back_event_t fin_or_event, void *callback_ctx, void *v_stream_ctx) {
        int ret = 0;

        auto *context = (FlowContext *) callback_ctx;

        switch (fin_or_event) {
            case picoquic_callback_stream_data:
                // fall through
            case picoquic_callback_stream_fin: {
                // get flow
                auto *flow = (flow::Flow *) v_stream_ctx;
                // create flow if not exists
                if (flow == nullptr) {
                    assert(length == sizeof(flow::FlowHeader));
                    // expect flow header
                    flow::FlowHeader header;
                    memcpy(&header, bytes, sizeof(flow::FlowHeader));

                    // connect to endpoint
                    net::ipv4::TcpConnection *connection = net::ipv4::TcpConnection::make(net::ipv4::SockAddr_In(header.ip, header.port));

                    // create new stream
                    flow = context->new_flow(stream_id, connection);
                    // set default path to TER
                    flow->set_stream_path_affinity(0);

                    return 0;
                }

                if (fin_or_event == picoquic_callback_stream_data) {
                    ret = flow->stream_data(bytes, length);
                } else {
                    ret = flow->stream_fin(bytes, length);
                }

                break;
            }
            case picoquic_callback_prepare_to_send: {
                // get flow from stream context
                auto *flow = (flow::Flow *) v_stream_ctx;
                // redirect
                ret = flow->prepare_to_send(bytes, length);

                break;
            }
            case picoquic_callback_stream_reset: {
                // delete flow
                context->delete_stream(stream_id);

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
            case picoquic_callback_almost_ready: {
                VLOG(3) << context->to_string() << " picoquic_callback_almost_ready";
                break;
            }
            case picoquic_callback_ready: {
                VLOG(3) << context->to_string() << " picoquic_callback_ready";

                if (context->is_client()) {
                    // probe new path (SAT)
                    struct sockaddr_storage addr_from;
                    int addr_from_is_name = 0;
                    struct sockaddr_storage addr_to;
                    int addr_to_is_name = 0;

                    picoquic_get_server_address("172.30.21.3", 6000, &addr_from, &addr_from_is_name); // remote_addr
                    picoquic_get_server_address("172.30.20.2", 0, &addr_to, &addr_to_is_name); // local_addr

                    context->probe_new_path_ex((struct sockaddr *) &addr_from, /*(struct sockaddr *) &addr_to*/nullptr,
                                               0, picoquic_current_time(), 0);

                    // subscribe to quality update
                }

                // add default (stream_id=0) path and subscribe to path quality updates
                auto *path = context->new_path(0);
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

                auto *path = context->new_path(stream_id);
                //ret = path->subscribe_to_quality_update_per_path(500000, 50000);

                VLOG(3) << path->to_string() << " picoquic_callback_path_available";

                break;
            }
            case picoquic_callback_path_suspended: {
                auto *path = (quic::Path *) v_stream_ctx;
                if (path == nullptr) {
                    // create new stream
                    path = context->new_path(stream_id);
                }

                VLOG(3) << path->to_string() << " picoquic_callback_path_suspended";

                // TODO implement, ?
                assert(false);

                break;
            }
            case picoquic_callback_path_deleted: {
                // TODO minimize if stream does not exists
                //uint64_t unique_path_id = stream_id; // stream_id = unique_path_id

                // TODO minimize, delete_path(stream_id) only
                auto *path = (quic::Path *) v_stream_ctx;
                if (path == nullptr) {
                    // create new stream
                    path = context->new_path(stream_id);
                }

                context->delete_path(stream_id);

                break;
            }
            case picoquic_callback_path_quality_changed: {
                auto *path = (quic::Path *) v_stream_ctx;
                if (path == nullptr) {
                    // create new stream
                    path = context->new_path(stream_id);
                }

                // update congestion
                path->get_path_quality(&path->_congestion);

                break;
            }
        }

        return ret;
    }
} // quic