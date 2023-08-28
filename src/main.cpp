//
// Created by Matthias Hofstätter on 04.04.23.
//

#include <iostream>
#include <netinet/tcp.h>
#include <glog/logging.h>

#include "args/ARGS.h"
#include "net/TcpListener.h"
#include "net/TcpConnection.h"
#include "net/SockAddr.h"
#include "proxy/Proxy.h"
#include "quic/Quic.h"
#include "quic/FlowContext.h"


int main(int argc, char *argv[]) {
    // init logging
    google::InitGoogleLogging(argv[0]);
    google::SetLogDestination(0,"mpp.log");
    FLAGS_alsologtostderr = true;

    // init arguments
    args::init(argc, argv);

    //net::ipv4::SockAddr_In sockaddr = net::ipv4::SockAddr_In(args::SAT_STRING);


    if (args::MODE == args::mode::LOCAL) { // local
        struct sockaddr_storage sockaddr;
        int ter_is_name = 0;

        picoquic_get_server_address("172.30.21.3", 6000, &sockaddr, &ter_is_name);

        quic::Quic *quic = quic::Quic::make(1, "ticket_store.bin");
        quic->set_default_congestion_algorithm(picoquic_bbr_algorithm);
        //quic->set_key_log_file_from_env();
        //quic->set_key_log_file("key.log");
        //quic->set_qlog(".");
        //quic->set_binlog(".");
        //quic->set_log_level(1);
        // debug
        quic->set_default_idle_timeout(10000000);
        // https://github.com/private-octopus/picoquic/blob/1e2979e8db0957c8ee798940091c4d0ef13bf8af/picoquic/picoquic.h#L1088
        quic->set_default_priority(0xA);
        // enable path callbacks
        quic->enable_path_callbacks_default(1);
        quic->set_default_multipath_option(2);
        // subscribe quality update
        quic->default_quality_update(0, 0);
        quic->set_padding_policy(39, 1);

        auto *flowContext = quic->create_context<quic::FlowContext>(picoquic_null_connection_id, picoquic_null_connection_id, (struct sockaddr *) &sockaddr, picoquic_current_time(), 0, "", QUIC_ALPN_FLOW, 1);
        flowContext->start_client_cnx();

        quic->start_packet_loop(0, sockaddr.ss_family, 0, 0, 0);

        proxy::Proxy proxy = proxy::Proxy(flowContext, net::ipv4::SockAddr_In("172.30.10.3", 5000));

        //exit when key pressed
        std::cin.ignore();
    } else { // remote
        quic::Quic *quic = quic::Quic::make(100, "/root/cert.pem", "/root/key.pem");

        quic->set_cookie_mode(2);
        quic->set_default_congestion_algorithm(picoquic_bbr_algorithm);
        //quic->set_qlog(".");
        //quic->set_log_level(1);
        //quic->set_key_log_file_from_env();
        //quic->set_key_log_file("key.log");
        //quic->set_binlog(".");
        // debug
        quic->set_default_idle_timeout(10000000);
        // https://github.com/private-octopus/picoquic/blob/1e2979e8db0957c8ee798940091c4d0ef13bf8af/picoquic/picoquic.h#L1088
        quic->set_default_priority(0xA);
        // enable path callbacks
        quic->enable_path_callbacks_default(1);
        quic->set_default_multipath_option(2);
        // subscribe quality update
        quic->default_quality_update(0, 0);
        quic->set_padding_policy(39, 1);

        quic->start_packet_loop(6000, 0, 0, 0, 0);

        //exit when key pressed
        std::cin.ignore();
    }

    return 0;
}