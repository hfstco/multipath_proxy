//
// Created by Matthias Hofstätter on 31.07.23.
//

#ifndef MULTIPATH_PROXY_QUICCONNECTION_H
#define MULTIPATH_PROXY_QUICCONNECTION_H

#include <future>

#include <picoquic.h>
#include <picosocks.h>
#include <picoquic_utils.h>
#include <autoqlog.h>
#include <picoquic_packet_loop.h>

#include "../worker/Looper.h"
#include "QuicConnection.h"

namespace net {

    class QuicServerConnection : public QuicConnection {
    public:
        QuicServerConnection(int port);

        std::string ToString();

        virtual ~QuicServerConnection();

    private:
        int _port;

        static int callback(picoquic_cnx_t* cnx,
                            uint64_t stream_id, uint8_t* bytes, size_t length,
                            picoquic_call_back_event_t fin_or_event, void* callback_ctx, void* v_stream_ctx);
        static int loop_callback(picoquic_quic_t* quic, picoquic_packet_loop_cb_enum cb_mode,
                                 void* callback_ctx, void * callback_arg);
    };
}


#endif //MULTIPATH_PROXY_QUICCONNECTION_H
