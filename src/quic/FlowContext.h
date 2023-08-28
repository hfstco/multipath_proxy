//
// Created by Matthias Hofstätter on 26.08.23.
//

#ifndef MPP_FLOWCONTEXT_H
#define MPP_FLOWCONTEXT_H

#include "Context.h"
#include "../net/SockAddr.h"
#include "../flow/Flow.h"

#define QUIC_ALPN_FLOW "flow"

namespace net::ipv4 {
    class TcpConnection;
}

namespace quic {

    class FlowContext : public Context {
        friend class Quic;
        friend class Context;
    public:
        flow::Flow *new_flow(net::ipv4::SockAddr_In source, net::ipv4::SockAddr_In destination, net::ipv4::TcpConnection *tcp_connection);

    private:
        explicit FlowContext(Quic *quic, picoquic_cnx_t *cnx);

        flow::Flow *new_flow(uint64_t stream_id, net::ipv4::SockAddr_In source, net::ipv4::SockAddr_In destination, net::ipv4::TcpConnection *tcp_connection);

        static int stream_data_cb(picoquic_cnx_t* cnx, uint64_t stream_id, uint8_t* bytes, size_t length, picoquic_call_back_event_t fin_or_event, void* callback_ctx, void* v_stream_ctx);
    };

} // quic

#endif //MPP_FLOWCONTEXT_H
