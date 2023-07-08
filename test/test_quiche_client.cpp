/*
//
// Created by Matthias Hofstätter on 05.07.23.
//

#include <gtest/gtest.h>
#include <glog/logging.h>

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>

#include <fcntl.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <quiche.h>

#include <thread>

#include "../src/net/SockAddr.h"

#define LOCAL_CONN_ID_LEN 16

#define MAX_DATAGRAM_SIZE 1350

static void debug_log(const char *line, void *argp) {
    fprintf(stderr, "%s\n", line);
}

quiche_conn *conn;
int sock;
net::ipv4::SockAddr_In local_addr;
socklen_t local_addr_len;

void send_loop() {
    while (1) {
        if(conn == NULL) {
            usleep(100);
            continue;
        }
        uint8_t out[MAX_DATAGRAM_SIZE];
        quiche_send_info send_info;
        ssize_t written = quiche_conn_send(conn, out, sizeof(out), &send_info);

        if (written == QUICHE_ERR_DONE) {
            //fprintf(stderr, "done writing\n");
            double t = quiche_conn_timeout_as_nanos(conn) / 1e9f;
            usleep(t);
            continue;
        }

        if (written < 0) {
            fprintf(stderr, "failed to create packet: %zd\n", written);
            return;
        }

        ssize_t sent = sendto(sock, out, written, 0,
                              (struct sockaddr *) &send_info.to,
                              send_info.to_len);

        if (sent != written) {
            perror("failed to send");
            return;
        }

        net::ipv4::SockAddr_In *sockAddrIn = (net::ipv4::SockAddr_In *) &send_info.to;

        LOG(INFO) << "sent " << sent << " bytes to " << sockAddrIn->ip() << ":" << sockAddrIn->port();
        fprintf(stderr, "sent %zd bytes\n", sent);
    }
}

void recv_loop() {
    static bool req_sent = false;

    static uint8_t buf[65535];

    while (1) {
        struct sockaddr_storage peer_addr;
        socklen_t peer_addr_len = sizeof(peer_addr);
        memset(&peer_addr, 0, peer_addr_len);

        ssize_t read = recvfrom(sock, buf, sizeof(buf), 0,
                                (struct sockaddr *) &peer_addr,
                                &peer_addr_len);

        if (read < 0) {
            */
/*if ((errno == EWOULDBLOCK) || (errno == EAGAIN)) {
                fprintf(stderr, "recv would block\n");
                break;
            }*//*


            perror("failed to read");
            return;
        }

        quiche_recv_info recv_info = {
                (struct sockaddr *) &peer_addr,
                peer_addr_len,
                (struct sockaddr *) &local_addr,
                local_addr_len,
        };

        ssize_t done = quiche_conn_recv(conn, buf, read, &recv_info);

        if (done < 0) {
            fprintf(stderr, "failed to process packet\n");
            continue;
        }

        fprintf(stderr, "recv %zd bytes\n", done);
    }

    fprintf(stderr, "done reading\n");

    if (quiche_conn_is_closed(conn)) {
        fprintf(stderr, "connection closed\n");

        return;
    }

    if (quiche_conn_is_established(conn) && !req_sent) {
        const uint8_t *app_proto;
        size_t app_proto_len;

        quiche_conn_application_proto(conn, &app_proto, &app_proto_len);

        fprintf(stderr, "connection established: %.*s\n",
                (int) app_proto_len, app_proto);

        const static uint8_t r[] = "GET /index.html\r\n";
        if (quiche_conn_stream_send(conn, 4, r, sizeof(r), true) < 0) {
            fprintf(stderr, "failed to send HTTP request\n");
            return;
        }

        fprintf(stderr, "sent HTTP request\n");

        req_sent = true;
    }

    if (quiche_conn_is_established(conn)) {
        uint64_t s = 0;

        quiche_stream_iter *readable = quiche_conn_readable(conn);

        while (quiche_stream_iter_next(readable, &s)) {
            fprintf(stderr, "stream %" PRIu64 " is readable\n", s);

            bool fin = false;
            ssize_t recv_len = quiche_conn_stream_recv(conn, s,
                                                       buf, sizeof(buf),
                                                       &fin);
            if (recv_len < 0) {
                break;
            }

            printf("%.*s", (int) recv_len, buf);

            if (fin) {
                if (quiche_conn_close(conn, true, 0, NULL, 0) < 0) {
                    fprintf(stderr, "failed to close connection\n");
                }
            }
        }

        quiche_stream_iter_free(readable);
    }
}

TEST(quiche, quiche_client) {
    const char *host = "127.0.0.1";
    const char *port = "8889";

    const struct addrinfo hints = {
            .ai_family = PF_UNSPEC,
            .ai_socktype = SOCK_DGRAM,
            .ai_protocol = IPPROTO_UDP
    };

    quiche_enable_debug_logging(debug_log, NULL);

    struct addrinfo *peer;
    if (getaddrinfo(host, port, &hints, &peer) != 0) {
        perror("failed to resolve host");
        return;
    }

    net::ipv4::SockAddr_In *peerSockAddr = (net::ipv4::SockAddr_In *)peer->ai_addr;
    LOG(INFO) << "PEER[ip=" << peerSockAddr->ip() << ", port=" << peerSockAddr->port() << "]";

    sock = socket(peer->ai_family, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("failed to create socket");
        return;
    }

    */
/*if (fcntl(sock, F_SETFL, O_NONBLOCK) != 0) {
        perror("failed to make socket non-blocking");
        return;
    }*//*


    quiche_config *config = quiche_config_new(QUICHE_PROTOCOL_VERSION);
    if (config == NULL) {
        fprintf(stderr, "failed to create config\n");
        return;
    }

    quiche_config_set_application_protos(config,
                                         (uint8_t *) "\x0ahq-interop\x05hq-29\x05hq-28\x05hq-27\x08http/0.9", 38);

    quiche_config_set_max_idle_timeout(config, 5000);
    quiche_config_set_max_recv_udp_payload_size(config, MAX_DATAGRAM_SIZE);
    quiche_config_set_max_send_udp_payload_size(config, MAX_DATAGRAM_SIZE);
    quiche_config_set_initial_max_data(config, 10000000);
    quiche_config_set_initial_max_stream_data_bidi_local(config, 1000000);
    quiche_config_set_initial_max_stream_data_uni(config, 1000000);
    quiche_config_set_initial_max_streams_bidi(config, 100);
    quiche_config_set_initial_max_streams_uni(config, 100);
    quiche_config_set_disable_active_migration(config, true);

    if (getenv("SSLKEYLOGFILE")) {
        quiche_config_log_keys(config);
    }

    uint8_t scid[LOCAL_CONN_ID_LEN];
    int rng = open("/dev/urandom", O_RDONLY);
    if (rng < 0) {
        perror("failed to open /dev/urandom");
        return;
    }

    ssize_t rand_len = read(rng, &scid, sizeof(scid));
    if (rand_len < 0) {
        perror("failed to create connection ID");
        return;
    }

    local_addr_len = sizeof(local_addr);
    if (getsockname(sock, (struct sockaddr *)&local_addr,
                    &local_addr_len) != 0)
    {
        perror("failed to get local address of socket");
        return;
    };

    LOG(INFO) << "SOCK[ip=" << local_addr.ip() << ", port=" << local_addr.port() << "]";

    conn = quiche_connect(host, (const uint8_t *) scid, sizeof(scid),
                                       (struct sockaddr *) &local_addr,
                                       local_addr_len,
                                       peer->ai_addr, peer->ai_addrlen, config);

    if (conn == NULL) {
        fprintf(stderr, "failed to create connection\n");
        return;
    }

    std::thread t_send([&] {
        send_loop();
    });

    std::thread t_recv([&] {
        recv_loop();
    });

    t_send.join();
    t_recv.join();

    freeaddrinfo(peer);
    quiche_conn_free(conn);
    quiche_config_free(config);
}*/
