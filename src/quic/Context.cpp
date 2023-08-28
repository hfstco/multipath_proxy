//
// Created by Matthias Hofstätter on 25.08.23.
//

#include "Context.h"

#include <glog/logging.h>
#include <tls_api.h>
#include <cassert>

#include "Quic.h"
#include "Path.h"
#include "Stream.h"
#include "FlowContext.h"

namespace quic {

    Context::Context(Quic *quic, picoquic_cnx_t *quic_cnx) : ContextBase((QuicBase *)quic, quic_cnx) {
        set_callback(Context::stream_data_cb, this);
    }

    Path *Context::new_path(uint64_t unique_path_id) {
        std::lock_guard lock(_mutex);

        // return existing path
        auto it = _paths.find(unique_path_id);
        if (it != _paths.end()) {
            return it->second;
        }

        // create new path
        Path *path = new Path(this, unique_path_id);
        _paths.insert({unique_path_id, path});

        return path;
    }

    void Context::delete_path(uint64_t unique_path_id) {
        std::lock_guard lock(_mutex);

        auto it = _paths.find(unique_path_id);

        // delete if path found
        if (it != _paths.end()) {
            delete it->second;
            it->second = nullptr;
            _paths.erase(it->first);
        }
    }

    void Context::delete_stream(uint64_t stream_id) {
        std::lock_guard lock(_mutex);

        auto it = _streams.find(stream_id);

        // delete if stream found
        if (it != _streams.end()) {
            delete it->second;
            it->second = nullptr;
            _streams.erase(it->first);
        }
    }

    int Context::stream_data_cb(picoquic_cnx_t *cnx, uint64_t stream_id, uint8_t *bytes, size_t length,
                                picoquic_call_back_event_t fin_or_event, void *callback_ctx, void *v_stream_ctx) {
        int ret = 0;

        // redirect to context
        if(strcmp(cnx->alpn, "flow") == 0) { // flow context
            ret = FlowContext::stream_data_cb(cnx, stream_id, bytes, length, fin_or_event, callback_ctx, v_stream_ctx);
        } else if(strcmp(cnx->alpn, "none") == 0) { // default context
            auto* quic = (Quic *)callback_ctx;

            auto* context = quic->get_context(cnx);
            if (context == nullptr) {
                // create flow
                context = quic->create_context(cnx);
            }

            auto* stream = (quic::Stream *)v_stream_ctx;
            if (stream == nullptr) {
                stream = context->new_stream(stream_id);
            }

            switch (fin_or_event) {
                case picoquic_callback_stream_data: {
                    VLOG(3) << quic->to_string() << "." << stream->to_string() << " picoquic_callback_stream_data";
                    break;
                }
                case picoquic_callback_stream_fin: {
                    VLOG(3) << quic->to_string() << "." << stream->to_string() << " picoquic_callback_stream_fin";
                    break;
                }
                case picoquic_callback_stream_reset: {
                    VLOG(3) << quic->to_string() << "." << stream->to_string() << " picoquic_callback_stream_reset";
                    break;
                }
                case picoquic_callback_stop_sending: {
                    VLOG(3) << quic->to_string() << "." << stream->to_string() << " picoquic_callback_stop_sending";
                    break;
                }
                case picoquic_callback_stateless_reset: {
                    VLOG(3) << quic->to_string() << "." << stream->to_string() << " picoquic_callback_stateless_reset";
                    break;
                }
                case picoquic_callback_close: {
                    VLOG(3) << quic->to_string() << "." << stream->to_string() << " picoquic_callback_close";
                    break;
                }
                case picoquic_callback_application_close: {
                    VLOG(3) << quic->to_string() << "." << stream->to_string() << " picoquic_callback_application_close";
                    break;
                }
                case picoquic_callback_stream_gap: {
                    VLOG(3) << quic->to_string() << "." << stream->to_string() << " picoquic_callback_stream_gap";
                    break;
                }
                case picoquic_callback_prepare_to_send: {
                    VLOG(3) << quic->to_string() << "." << stream->to_string() << " picoquic_callback_prepare_to_send";
                    break;
                }
                case picoquic_callback_almost_ready: {
                    VLOG(3) << quic->to_string() << "." << stream->to_string() << " picoquic_callback_almost_ready";
                    break;
                }
                case picoquic_callback_ready: {
                    VLOG(3) << quic->to_string() << "." << stream->to_string() << " picoquic_callback_ready";
                    break;
                }
                case picoquic_callback_datagram: {
                    VLOG(3) << quic->to_string() << "." << stream->to_string() << " picoquic_callback_datagram";
                    break;
                }
                case picoquic_callback_version_negotiation: {
                    VLOG(3) << quic->to_string() << "." << stream->to_string() << " picoquic_callback_version_negotiation";
                    break;
                }
                case picoquic_callback_request_alpn_list: {
                    VLOG(3) << quic->to_string() << "." << stream->to_string() << " picoquic_callback_request_alpn_list";
                    break;
                }
                case picoquic_callback_set_alpn: {
                    VLOG(3) << quic->to_string() << "." << stream->to_string() << " picoquic_callback_set_alpn";
                    break;
                }
                case picoquic_callback_pacing_changed: {
                    VLOG(3) << quic->to_string() << "." << stream->to_string() << " picoquic_callback_pacing_changed";
                    break;
                }
                case picoquic_callback_prepare_datagram: {
                    VLOG(3) << quic->to_string() << "." << stream->to_string() << " picoquic_callback_prepare_datagram";
                    break;
                }
                case picoquic_callback_datagram_acked: {
                    VLOG(3) << quic->to_string() << "." << stream->to_string() << " picoquic_callback_datagram_acked";
                    break;
                }
                case picoquic_callback_datagram_lost: {
                    VLOG(3) << quic->to_string() << "." << stream->to_string() << " picoquic_callback_datagram_lost";
                    break;
                }
                case picoquic_callback_datagram_spurious: {
                    VLOG(3) << quic->to_string() << "." << stream->to_string() << " picoquic_callback_datagram_spurious";
                    break;
                }
                case picoquic_callback_path_available: {
                    VLOG(3) << quic->to_string() << "." << stream->to_string() << " picoquic_callback_path_available";
                    break;
                }
                case picoquic_callback_path_suspended: {
                    VLOG(3) << quic->to_string() << "." << stream->to_string() << " picoquic_callback_path_suspended";
                    break;
                }
                case picoquic_callback_path_deleted: {
                    VLOG(3) << quic->to_string() << "." << stream->to_string() << " picoquic_callback_path_deleted";
                    break;
                }
                case picoquic_callback_path_quality_changed: {
                    VLOG(3) << quic->to_string() << "." << stream->to_string() << " picoquic_callback_path_quality_changed";
                    break;
                }
            }
        }

        return ret;
    }

    int Context::stream_direct_receive(picoquic_cnx_t *cnx, uint64_t stream_id, int fin, const uint8_t *bytes,
                                       uint64_t offset, size_t length, void *direct_receive_ctx) {
        assert(false);
        return 0;
    }

    std::string Context::to_string() {
        return "Context[]";
    }

    Context::~Context() {
        std::lock_guard lock(_mutex);

        // delete streams
        for(auto& it : _streams) {
            delete it.second;
            _streams.erase(it.first);
        }

        // delete paths
        for(auto &it : _paths) {
            delete it.second;
            _paths.erase(it.first);
        }
    }
} // picoquic