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
#include <thread>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include "../src/net/SockAddr.h"

#include "quiche.h"

#define LOCAL_CONN_ID_LEN 16

#define MAX_DATAGRAM_SIZE 1350

#define MAX_TOKEN_LEN \
    sizeof("quiche") - 1 + \
    sizeof(struct sockaddr_storage) + \
    QUICHE_MAX_CONN_ID_LEN

static uint8_t *gen_cid(uint8_t *cid, size_t cid_len) {
    int rng = open("/dev/urandom", O_RDONLY);
    if (rng < 0) {
        perror("failed to open /dev/urandom");
        return NULL;
    }

    ssize_t rand_len = read(rng, cid, cid_len);
    if (rand_len < 0) {
        perror("failed to create connection ID");
        return NULL;
    }

    return cid;
}

static void debug_log(const char *line, void *argp) {
    fprintf(stderr, "%s\n", line);
}

static void mint_token(const uint8_t *dcid, size_t dcid_len,
                       struct sockaddr *addr, socklen_t addr_len,
                       uint8_t *token, size_t *token_len) {
    memcpy(token, "quiche", sizeof("quiche") - 1);
    memcpy(token + sizeof("quiche") - 1, addr, addr_len);
    memcpy(token + sizeof("quiche") - 1 + addr_len, dcid, dcid_len);

    *token_len = sizeof("quiche") - 1 + addr_len + dcid_len;
}

static bool validate_token(const uint8_t *token, size_t token_len,
                           struct sockaddr *addr, socklen_t addr_len,
                           uint8_t *odcid, size_t *odcid_len) {
    if ((token_len < sizeof("quiche") - 1) ||
        memcmp(token, "quiche", sizeof("quiche") - 1)) {
        return false;
    }

    token += sizeof("quiche") - 1;
    token_len -= sizeof("quiche") - 1;

    if ((token_len < addr_len) || memcmp(token, addr, addr_len)) {
        return false;
    }

    token += addr_len;
    token_len -= addr_len;

    if (*odcid_len < token_len) {
        return false;
    }

    memcpy(odcid, token, token_len);
    *odcid_len = token_len;

    return true;
}

int s_sock;
quiche_conn *s_conn = NULL;
net::ipv4::SockAddr_In s_local_addr;
socklen_t s_local_addr_len;
quiche_config *s_config;

static quiche_conn *create_conn(uint8_t *scid, size_t scid_len,
                                   uint8_t *odcid, size_t odcid_len,
                                   struct sockaddr *local_addr,
                                   socklen_t local_addr_len,
                                   struct sockaddr *peer_addr,
                                   socklen_t peer_addr_len)
{
    if (scid_len != LOCAL_CONN_ID_LEN) {
        fprintf(stderr, "failed, scid length too short\n");
    }

    quiche_conn *c = quiche_accept(scid, LOCAL_CONN_ID_LEN,
                                      odcid, odcid_len,
                                      local_addr,
                                      local_addr_len,
                                      (struct sockaddr *) peer_addr,
                                      peer_addr_len,
                                      s_config);

    if (c == NULL) {
        fprintf(stderr, "failed to create connection\n");
        return NULL;
    }

memcpy(&conn_io->peer_addr, peer_addr, peer_addr_len);
    conn_io->peer_addr_len = peer_addr_len;


    fprintf(stderr, "new connection\n");

    return c;
}

static void send_loop() {
    uint8_t out[MAX_DATAGRAM_SIZE];

    quiche_send_info send_info;

    while (1) {
        if(s_conn == NULL) {
            //LOG(INFO) << "send_loop loop";
            usleep(100);
            continue;
        }

        ssize_t written = quiche_conn_send(s_conn, out, sizeof(out),
                                           &send_info);

        if (written == QUICHE_ERR_DONE) {
            fprintf(stderr, "done writing\n");
            break;
        }

        if (written < 0) {
            fprintf(stderr, "failed to create packet: %zd\n", written);
            return;
        }

        ssize_t sent = sendto(s_sock, out, written, 0,
                              (struct sockaddr *) &send_info.to,
                              send_info.to_len);

        if (sent != written) {
            perror("failed to send");
            return;
        }

        fprintf(stderr, "sent %zd bytes\n", sent);
    }

    double t = quiche_conn_timeout_as_nanos(s_conn) / 1e9f;
    usleep(t);
}


static void recv_loop() {
    uint8_t buf[65535];
    uint8_t out[MAX_DATAGRAM_SIZE];

    while (1) {
        net::ipv4::SockAddr_In peer_addr;
        socklen_t peer_addr_len = sizeof(peer_addr);

        ssize_t read = recvfrom(s_sock, buf, sizeof(buf), 0, (struct sockaddr *) &peer_addr, &peer_addr_len);

        if (read < 0) {
if ((errno == EWOULDBLOCK) || (errno == EAGAIN)) {
                fprintf(stderr, "recv would block\n");
                break;
            }


            perror("failed to read");
            return;
        }

        uint8_t type;
        uint32_t version;

        uint8_t scid[QUICHE_MAX_CONN_ID_LEN];
        size_t scid_len = sizeof(scid);

        uint8_t dcid[QUICHE_MAX_CONN_ID_LEN];
        size_t dcid_len = sizeof(dcid);

        uint8_t odcid[QUICHE_MAX_CONN_ID_LEN];
        size_t odcid_len = sizeof(odcid);

        uint8_t token[MAX_TOKEN_LEN];
        size_t token_len = sizeof(token);

        int rc = quiche_header_info(buf, read, LOCAL_CONN_ID_LEN, &version,
                                    &type, scid, &scid_len, dcid, &dcid_len,
                                    token, &token_len);
        if (rc < 0) {
            fprintf(stderr, "failed to parse header: %d\n", rc);
            continue;
        }

        if (s_conn == NULL) {
            if (!quiche_version_is_supported(version)) {
                fprintf(stderr, "version negotiation\n");

                ssize_t written = quiche_negotiate_version(scid, scid_len,
                                                           dcid, dcid_len,
                                                           out, sizeof(out));

                if (written < 0) {
                    fprintf(stderr, "failed to create vneg packet: %zd\n",
                            written);
                    continue;
                }

                ssize_t sent = sendto(s_sock, out, written, 0,
                                      (struct sockaddr *) &peer_addr,
                                      peer_addr_len);
                if (sent != written) {
                    perror("failed to send");
                    continue;
                }

                fprintf(stderr, "sent %zd bytes\n", sent);
                continue;
            }

            if (token_len == 0) {
                fprintf(stderr, "stateless retry\n");

                mint_token(dcid, dcid_len, (struct sockaddr*) &peer_addr, peer_addr_len,
                           token, &token_len);

                uint8_t new_cid[LOCAL_CONN_ID_LEN];

                if (gen_cid(new_cid, LOCAL_CONN_ID_LEN) == NULL) {
                    continue;
                }

                ssize_t written = quiche_retry(scid, scid_len,
                                               dcid, dcid_len,
                                               new_cid, LOCAL_CONN_ID_LEN,
                                               token, token_len,
                                               version, out, sizeof(out));

                if (written < 0) {
                    fprintf(stderr, "failed to create retry packet: %zd\n",
                            written);
                    continue;
                }

                ssize_t sent = sendto(s_sock, out, written, 0,
                                      (struct sockaddr *) &peer_addr,
                                      peer_addr_len);
                if (sent != written) {
                    perror("failed to send");
                    continue;
                }

                fprintf(stderr, "sent %zd bytes\n", sent);
                continue;
            }


            if (!validate_token(token, token_len, (sockaddr *) &peer_addr, peer_addr_len,
                                odcid, &odcid_len)) {
                fprintf(stderr, "invalid address validation token\n");
                continue;
            }

            s_conn = create_conn(dcid, dcid_len, odcid, odcid_len,
                               (struct sockaddr *) &s_local_addr, s_local_addr_len,
                               (struct sockaddr *) &peer_addr, peer_addr_len);

            if (s_conn == NULL) {
                continue;
            }
        }

        quiche_recv_info recv_info = {
                (struct sockaddr *)&peer_addr,
                peer_addr_len,

                (struct sockaddr *)&s_local_addr,
                s_local_addr_len,
        };

        ssize_t done = quiche_conn_recv(s_conn, buf, read, &recv_info);

        if (done < 0) {
            fprintf(stderr, "failed to process packet: %zd\n", done);
            continue;
        }

        fprintf(stderr, "recv %zd bytes\n", done);

        if (quiche_conn_is_established(s_conn)) {
            uint64_t s = 0;

            quiche_stream_iter *readable = quiche_conn_readable(s_conn);

            while (quiche_stream_iter_next(readable, &s)) {
                fprintf(stderr, "stream %" PRIu64 " is readable\n", s);

                bool fin = false;
                ssize_t recv_len = quiche_conn_stream_recv(s_conn, s,
                                                           buf, sizeof(buf),
                                                           &fin);
                if (recv_len < 0) {
                    break;
                }

                if (fin) {
                    static const char *resp = "byez\n";
                    quiche_conn_stream_send(s_conn, s, (uint8_t *) resp,
                                            5, true);
                }
            }

            quiche_stream_iter_free(readable);
        }
    }

HASH_ITER(hh, conns->h, conn_io, tmp) {
        flush_egress(loop, conn_io);

        if (quiche_conn_is_closed(conn_io->conn)) {
            quiche_stats stats;
            quiche_path_stats path_stats;

            quiche_conn_stats(conn_io->conn, &stats);
            quiche_conn_path_stats(conn_io->conn, 0, &path_stats);

            fprintf(stderr, "connection closed, recv=%zu sent=%zu lost=%zu rtt=%" PRIu64 "ns cwnd=%zu\n",
                    stats.recv, stats.sent, stats.lost, path_stats.rtt, path_stats.cwnd);

            HASH_DELETE(hh, conns->h, conn_io);

            ev_timer_stop(loop, &conn_io->timer);
            quiche_conn_free(conn_io->conn);
            free(conn_io);
        }
    }

}

TEST(quiche, quiche_server) {
    const char *host = "127.0.0.1";
    const char *port = "8889";

    const struct addrinfo hints = {
            .ai_family = PF_UNSPEC,
            .ai_socktype = SOCK_DGRAM,
            .ai_protocol = IPPROTO_UDP
    };

    quiche_enable_debug_logging(debug_log, NULL);

    struct addrinfo *local;
    if (getaddrinfo(host, port, &hints, &local) != 0) {
        perror("failed to resolve host");
        return;
    }

    s_sock = socket(local->ai_family, SOCK_DGRAM, 0);
    if (s_sock < 0) {
        perror("failed to create socket");
        return;
    }

    if (bind(s_sock, local->ai_addr, local->ai_addrlen) < 0) {
        perror("failed to connect socket");
        return;
    }

    s_config = quiche_config_new(QUICHE_PROTOCOL_VERSION);
    if (s_config == NULL) {
        fprintf(stderr, "failed to create config\n");
        return;
    }

    quiche_config_load_cert_chain_from_pem_file(s_config, "./cert.crt");
    quiche_config_load_priv_key_from_pem_file(s_config, "./cert.key");

    quiche_config_set_application_protos(s_config,
                                         (uint8_t *) "\x0ahq-interop\x05hq-29\x05hq-28\x05hq-27\x08http/0.9", 38);

    quiche_config_set_max_idle_timeout(s_config, 5000);
    quiche_config_set_max_recv_udp_payload_size(s_config, MAX_DATAGRAM_SIZE);
    quiche_config_set_max_send_udp_payload_size(s_config, MAX_DATAGRAM_SIZE);
    quiche_config_set_initial_max_data(s_config, 10000000);
    quiche_config_set_initial_max_stream_data_bidi_local(s_config, 1000000);
    quiche_config_set_initial_max_stream_data_bidi_remote(s_config, 1000000);
    quiche_config_set_initial_max_streams_bidi(s_config, 100);
    quiche_config_set_cc_algorithm(s_config, QUICHE_CC_RENO);

    //connect
    uint8_t buf[65535];
    uint8_t out[MAX_DATAGRAM_SIZE];

    net::ipv4::SockAddr_In peer_addr;
    socklen_t peer_addr_len = sizeof(peer_addr);

    ssize_t read = recvfrom(s_sock, buf, sizeof(buf), 0, (struct sockaddr *) &peer_addr, &peer_addr_len);

    if (read < 0) {
if ((errno == EWOULDBLOCK) || (errno == EAGAIN)) {
            fprintf(stderr, "recv would block\n");
            break;
        }


        perror("failed to read");
        return;
    }

    uint8_t type;
    uint32_t version;

    uint8_t scid[QUICHE_MAX_CONN_ID_LEN];
    size_t scid_len = sizeof(scid);

    uint8_t dcid[QUICHE_MAX_CONN_ID_LEN];
    size_t dcid_len = sizeof(dcid);

    uint8_t odcid[QUICHE_MAX_CONN_ID_LEN];
    size_t odcid_len = sizeof(odcid);

    uint8_t token[MAX_TOKEN_LEN];
    size_t token_len = sizeof(token);

    int rc = quiche_header_info(buf, read, LOCAL_CONN_ID_LEN, &version,
                                &type, scid, &scid_len, dcid, &dcid_len,
                                token, &token_len);
    if (rc < 0) {
        fprintf(stderr, "failed to parse header: %d\n", rc);
    }

    if (token_len == 0) {
        fprintf(stderr, "stateless retry\n");

        mint_token(dcid, dcid_len, (struct sockaddr*) &peer_addr, peer_addr_len,
                   token, &token_len);

        uint8_t new_cid[LOCAL_CONN_ID_LEN];

        if (gen_cid(new_cid, LOCAL_CONN_ID_LEN) == NULL) {
            LOG(ERROR) << "gen_cid failed";
        }

        ssize_t written = quiche_retry(scid, scid_len,
                                       dcid, dcid_len,
                                       new_cid, LOCAL_CONN_ID_LEN,
                                       token, token_len,
                                       version, out, sizeof(out));

        if (written < 0) {
            fprintf(stderr, "failed to create retry packet: %zd\n",
                    written);
            return;
        }

        ssize_t sent = sendto(s_sock, out, written, 0,
                              (struct sockaddr *) &peer_addr,
                              peer_addr_len);
        if (sent != written) {
            perror("failed to send");
            return;
        }

        fprintf(stderr, "sent %zd bytes\n", sent);
    }

    freeaddrinfo(local);
    quiche_config_free(s_config);
}
*/
