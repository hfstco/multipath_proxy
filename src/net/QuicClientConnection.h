//
// Created by Matthias Hofstätter on 31.07.23.
//

#ifndef MPP_QUICCLIENTCONNECTION_H
#define MPP_QUICCLIENTCONNECTION_H


#include <cstdint>
#include <cstdio>

#include <picoquic_packet_loop.h>

#include "QuicConnection.h"

namespace net {

    class QuicClientConnection : public QuicConnection {
    public:
        QuicClientConnection(std::string server_name, int port, bool is_sat, std::string ticket_store_name, std::string token_store_name);

        std::string to_string() override;

        virtual ~QuicClientConnection();

    private:
        std::string _server_name;
        int _port;
        std::string _ticket_store_filename;
        std::string _token_store_filename;
        std::string _sni;
        struct sockaddr_storage _server_address;
        std::atomic_flag _ready;

        static int callback(picoquic_cnx_t* cnx,
                            uint64_t stream_id, uint8_t* bytes, size_t length,
                            picoquic_call_back_event_t fin_or_event, void* callback_ctx, void* v_stream_ctx);
        static int loop_callback(picoquic_quic_t* quic, picoquic_packet_loop_cb_enum cb_mode,
                                 void* callback_ctx, void * callback_arg);
    };

} // net

#endif //MPP_QUICCLIENTCONNECTION_H
