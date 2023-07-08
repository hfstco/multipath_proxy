/*
//
// Created by Matthias Hofstätter on 05.07.23.
//

#include "QuicConnection.h"

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

namespace net {
    QuicConnection::QuicConnection(net::ipv4::SockAddr_In sockAddr, bool listener) : recvLooper_(std::bind(&QuicConnection::RecvLoop, this)),
                                                                                                                      sendLooper_(std::bind(&QuicConnection::SendLoop, this)),
                                                                                                                      conn_(NULL) {
        quiche_enable_debug_logging(debug_log, NULL);
        sockAddrLen_ = sizeof(sockAddr);
        peerAddrLen_ = sizeof(sockAddr);

        if (listener) {
            // server
            sockAddr_ = sockAddr;

            sock_ = socket(sockAddr_.family(), SOCK_DGRAM, 0);
            if (sock_ < 0) {
                perror("failed to create socket");
                return;
            }

            if (bind(sock_, (struct sockaddr *) &sockAddr_, sockAddrLen_) < 0) {
                perror("failed to connect socket");
                return;
            }

            config_ = quiche_config_new(QUICHE_PROTOCOL_VERSION);
            if (config_ == NULL) {
                fprintf(stderr, "failed to create config\n");
                return;
            }

            quiche_config_load_cert_chain_from_pem_file(config_, "./cert.crt");
            quiche_config_load_priv_key_from_pem_file(config_, "./cert.key");

            quiche_config_set_application_protos(config_,
                                                 (uint8_t *) "\x0ahq-interop\x05hq-29\x05hq-28\x05hq-27\x08http/0.9", 38);

            quiche_config_set_max_idle_timeout(config_, 5000);
            quiche_config_set_max_recv_udp_payload_size(config_, MAX_DATAGRAM_SIZE);
            quiche_config_set_max_send_udp_payload_size(config_, MAX_DATAGRAM_SIZE);
            quiche_config_set_initial_max_data(config_, 10000000);
            quiche_config_set_initial_max_stream_data_bidi_local(config_, 1000000);
            quiche_config_set_initial_max_stream_data_bidi_remote(config_, 1000000);
            quiche_config_set_initial_max_streams_bidi(config_, 100);
            quiche_config_set_cc_algorithm(config_, QUICHE_CC_RENO);
        } else {
            // client
            peerAddr_ = sockAddr;

            sock_ = socket(AF_INET, SOCK_DGRAM, 0);
            if (sock_ < 0) {
                perror("failed to create socket");
                return;
            }

            config_ = quiche_config_new(0xbabababa);
            if (config_ == NULL) {
                fprintf(stderr, "failed to create config\n");
                return;
            }

            quiche_config_set_application_protos(config_,
                                                 (uint8_t *) "\x0ahq-interop\x05hq-29\x05hq-28\x05hq-27\x08http/0.9", 38);

            quiche_config_set_max_idle_timeout(config_, 5000);
            quiche_config_set_max_recv_udp_payload_size(config_, MAX_DATAGRAM_SIZE);
            quiche_config_set_max_send_udp_payload_size(config_, MAX_DATAGRAM_SIZE);
            quiche_config_set_initial_max_data(config_, 10000000);
            quiche_config_set_initial_max_stream_data_bidi_local(config_, 1000000);
            quiche_config_set_initial_max_stream_data_uni(config_, 1000000);
            quiche_config_set_initial_max_streams_bidi(config_, 100);
            quiche_config_set_initial_max_streams_uni(config_, 100);
            quiche_config_set_disable_active_migration(config_, true);
            //quiche_config_verify_peer(config_, false);

            */
/*if (getenv("SSLKEYLOGFILE")) {
                quiche_config_log_keys(config_);
            }*//*


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

            if (getsockname(sock_, (struct sockaddr *) &sockAddr_,
                            &sockAddrLen_) != 0)
            {
                perror("failed to get local address of socket");
                return;
            };

            conn_ = quiche_connect("127.0.0.1", (const uint8_t *) scid, sizeof(scid),
                                  (struct sockaddr *) &sockAddr_, sockAddrLen_,
                                          (struct sockaddr *) &peerAddr_, peerAddrLen_, config_);

            if (conn_ == NULL) {
                fprintf(stderr, "failed to create connection\n");
                return;
            }

            sendLooper_.Start();
        }

        recvLooper_.Start();
    }

    void QuicConnection::RecvLoop() {
        uint8_t buf[65535];
        uint8_t out[MAX_DATAGRAM_SIZE];
        net::ipv4::SockAddr_In peerAddr;
        socklen_t peerAddrLen = sizeof(peerAddr);

        ssize_t read = recvfrom(sock_, buf, sizeof(buf), 0, (struct sockaddr *) &peerAddr, &peerAddrLen);
        if (read < 0) {
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
            return;
        }

        if (conn_ == NULL) {
            LOG(INFO) << "read " << read << " bytes from " << peerAddr.ip() << ":" << peerAddr.port();

            if (!quiche_version_is_supported(version)) {
                LOG(INFO) << "VERSION NEGOTIATION";

                ssize_t written = quiche_negotiate_version(scid, scid_len,
                                                           dcid, dcid_len,
                                                           out, sizeof(out));

                if (written < 0) {
                    fprintf(stderr, "failed to create vneg packet: %zd\n",
                            written);
                    return;
                }

                ssize_t sent = sendto(sock_, out, written, 0,
                                      (struct sockaddr *) &peerAddr,
                                      peerAddrLen);
                if (sent != written) {
                    perror("failed to send");
                    return;
                }

                LOG(INFO) << "sent " << sent << " bytes to " << peerAddr.ip() << ":" << peerAddr.port();

                return;
            }

            if (token_len == 0) {
                LOG(INFO) << "STATELESS RETRY";

                mint_token(dcid, dcid_len, (struct sockaddr*) &peerAddr, peerAddrLen,
                           token, &token_len);

                uint8_t new_cid[LOCAL_CONN_ID_LEN];

                if (gen_cid(new_cid, LOCAL_CONN_ID_LEN) == NULL) {
                    return;
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

                ssize_t sent = sendto(sock_, out, written, 0, (struct sockaddr *) &peerAddr, peerAddrLen);
                if (sent != written) {
                    perror("failed to send");
                    return;
                }

                LOG(INFO) << "sent " << sent << " bytes to " << peerAddr.ip() << ":" << peerAddr.port();
                return;
            }


            if (!validate_token(token, token_len, (sockaddr *) &peerAddr, peerAddrLen, odcid, &odcid_len)) {
                fprintf(stderr, "invalid address validation token\n");
                return;
            }

            // create conn
            if (scid_len != LOCAL_CONN_ID_LEN) {
                fprintf(stderr, "failed, scid length too short\n");
            }

            connMutex_.lock();
            conn_ = quiche_accept(scid, LOCAL_CONN_ID_LEN,
                                           odcid, odcid_len,
                                           (struct sockaddr *) &sockAddr_,
                                           sockAddrLen_,
                                           (struct sockaddr *) &peerAddr,
                                           peerAddrLen,
                                           config_);
            connMutex_.unlock();
            if (conn_ == NULL) {
                fprintf(stderr, "failed to create connection\n");
                return;
            }

            LOG(INFO) << "NEW CONNECTION " << sockAddr_.ip() << ":" << sockAddr_.port() << " -> " << peerAddr.ip() << ":" << peerAddr.port();
            sendLooper_.Start();
        }

        quiche_recv_info recv_info = {
                (struct sockaddr *)&peerAddr,
                peerAddrLen,
                (struct sockaddr *)&sockAddr_,
                sockAddrLen_,
        };

        connMutex_.lock();
        ssize_t done = quiche_conn_recv(conn_, buf, read, &recv_info);
        connMutex_.unlock();
        if (done < 0) {
            fprintf(stderr, "failed to process packet: %zd\n", done);
            return;
        }

        LOG(INFO) << "read " << done << " bytes from " << peerAddr.ip() << ":" << peerAddr.port();

        {
            std::lock_guard lock(connMutex_);

            if (quiche_conn_is_closed(conn_)) {
                LOG(INFO) << "CONNECTION CLOSED";
                return;
            }
        }

        if (quiche_conn_is_established(conn_)) {
            uint64_t s = 0;

            quiche_stream_iter *readable = quiche_conn_readable(conn_);

            while (quiche_stream_iter_next(readable, &s)) {
                fprintf(stderr, "stream %" PRIu64 " is readable\n", s);

                bool fin = false;
                ssize_t recv_len = quiche_conn_stream_recv(conn_, s,
                                                           buf, sizeof(buf),
                                                           &fin);
                if (recv_len < 0) {
                    break;
                }

                if (fin) {
                    static const char *resp = "byez\n";
                    quiche_conn_stream_send(conn_, s, (uint8_t *) resp,
                                            5, true);
                }
            }

            quiche_stream_iter_free(readable);
        }
    }

    void QuicConnection::SendLoop() {
        uint8_t out[MAX_DATAGRAM_SIZE];
        quiche_send_info send_info;

        connMutex_.lock();
        ssize_t written = quiche_conn_send(conn_, out, sizeof(out), &send_info);
        connMutex_.unlock();
        if (written == QUICHE_ERR_DONE) {
            //double t = quiche_conn_timeout_as_nanos(conn_) / 1e9f;
            usleep(1);
            return;
        }
        if (written < 0) {
            fprintf(stderr, "failed to create packet: %zd\n", written);
            return;
        }

        ssize_t sent = sendto(sock_, out, written, 0, (struct sockaddr *) &send_info.to, send_info.to_len);
        if (sent != written) {
            perror("failed to send");
            return;
        }

        // debug
        net::ipv4::SockAddr_In *sockAddr = (net::ipv4::SockAddr_In *) &send_info.to;
        LOG(INFO) << "sent " << sent << " bytes to " << sockAddr->ip() << ":" << sockAddr->port();
        //fprintf(stderr, "sent %zd bytes\n", sent);

        //double t = quiche_conn_timeout_as_nanos(conn_) / 1e9f;
        usleep(1);
    }
} // net*/
