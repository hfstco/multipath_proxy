//
// Created by Matthias Hofstätter on 14.04.23.
//

#include <thread>
#include <cassert>
#include <future>
#include <fcntl.h>
#include <csignal>

#include "Flow.h"

#include "../net/TcpConnection.h"
#include "../quic/Stream.h"
#include "../quic/FlowContext.h"
#include "picoquic_internal.h"
#include "../backlog/Chunk.h"
#include "../backlog/TotalBacklog.h"
#include "../threadpool/Threadpool.h"

namespace flow {

    Flow::Flow(quic::FlowContext *context, uint64_t stream_id, net::ipv4::TcpConnection *connection) : Stream((quic::Context *) context, stream_id),
                                                                                                       _connection(connection),
                                                                                                       _active(false),
                                                                                                       _path(0) {
        VLOG(1) << "*" << to_string();
        LOG(INFO) << "New Flow width ID=" << _stream_id;

        int ret = 0;
        // pipes
        if ((ret = pipe2(_rx_pipe, O_NONBLOCK)) == -1) {
            assert(false); // not implemented
        }
        if ((ret = fcntl(_rx_pipe[0], F_SETPIPE_SZ, 1000000)) == -1) {
            assert(false); // not implemented
        }
        if ((ret = fcntl(_rx_pipe[1], F_SETPIPE_SZ, 1000000)) == -1) {
            assert(false); // not implemented
        }

        if ((ret = pipe2(_tx_pipe, O_NONBLOCK)) == -1) {
            assert(false);
        }
        if ((ret = fcntl(_tx_pipe[0], F_SETPIPE_SZ, 1000000)) == -1) {
            assert(false); // not implemented
        }
        if ((ret = fcntl(_tx_pipe[1], F_SETPIPE_SZ, 1000000)) == -1) {
            assert(false); // not implemented
        }

    }

    void Flow::recv_from_connection() {
        std::lock_guard lock(_rx_mutex);

        if (_backlog > 100000) {
            return;
        }

        ssize_t bytes_spliced = 0;
        if ((bytes_spliced = splice(_connection->fd(), nullptr, _rx_pipe[1], nullptr, 4096, SPLICE_F_MOVE | SPLICE_F_NONBLOCK)) == -1) {
            if (errno == EAGAIN) {
                THREADPOOL.push_task(&Flow::recv_from_connection, this);
                return;
            }
            LOG(INFO) << "recv_from_connection splice errno=" << errno;

            LOG(INFO) << "CLOSED recv_from_connection";

            // mark stream active if inactive
            bool expected = false;
            if (_active.compare_exchange_strong(expected, true)) {
                LOG(INFO) << "STREAM ACTIVE CHANGED: TRUE";
                mark_active_stream(1, this);
            }

            close(_rx_pipe[1]);
            close(_rx_pipe[0]);
            return;
        };

        LOG(INFO) << "SPLICED IN " << bytes_spliced << " bytes";

        _backlog += bytes_spliced;
        TOTAL_BACKLOG += bytes_spliced;

        // mark stream active if inactive
        bool expected = false;
        if (_active.compare_exchange_strong(expected, true)) {
            LOG(INFO) << "STREAM ACTIVE CHANGED: TRUE";
            mark_active_stream(1, this);
        }

        // close pipe if connection closed
        if (bytes_spliced == 0) {
            LOG(INFO) << "CLOSED recv_from_connection";

            close(_rx_pipe[1]);
            close(_rx_pipe[0]);
            return;
        }

        THREADPOOL.push_task(&Flow::recv_from_connection, this);
    }

    void Flow::send_to_connection() {
        ssize_t bytes_spliced = 0;
        if ((bytes_spliced = splice(_tx_pipe[0], nullptr, _connection->fd(), nullptr, 1024, SPLICE_F_MOVE)) == -1) {
            if (errno == EAGAIN) {
                THREADPOOL.push_task(&Flow::send_to_connection, this);
                return;
            }

            LOG(INFO) << "send_to_connection splice errno=" << errno;
        }

        LOG(INFO) << "SPLICED OUT " << bytes_spliced << " bytes";

        if (bytes_spliced == 0 || bytes_spliced == -1) {
            LOG(INFO) << "CLOSED send_to_connection";

            if (_closed) {
                THREADPOOL.push_task(&quic::FlowContext::delete_flow, (quic::FlowContext*) _context, _stream_id);
            } else {
                _closed = true;
                _connection->Close();
            }

            return;
        }

        THREADPOOL.push_task(&Flow::send_to_connection, this);
    }

    int Flow::prepare_to_send(uint8_t *bytes, size_t length) {
        int ret = 0;

        std::lock_guard lock(_rx_mutex);

        // update path affinity
        if ((_backlog <= 2000 || TOTAL_BACKLOG <= 74219)) { // && std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - flow->last_satellite_timestamp.load()).count() > 600) {
            // switch to ter
            uint64_t path = 1;
            if (_path.compare_exchange_strong(path, 0)) {
                ret = set_stream_path_affinity(0); // TER

                LOG(INFO) << "SWITCH TO TER";
            }
        } else {
            // switch to sat
            // TODO is available?
            uint64_t path = 0;
            if (_path.compare_exchange_strong(path, 1)) {
                ret = set_stream_path_affinity(1); // SAT

                LOG(INFO) << "SWITCH TO SAT";
            }
        }

        // get max size
        auto *data = (uint8_t*) malloc(length);
        ssize_t size = 0;
        if ((size = read(_rx_pipe[0], data, length)) == -1) { // avoid unnecessary memcpy
            LOG(INFO) << "prepare_to_send read errno=" << errno;
            if (errno == EAGAIN) {
                // no data available, mark flow/stream inactive
                bool expected = true;
                if (_active.compare_exchange_strong(expected, false)) {
                    picoquic_provide_stream_data_buffer(bytes, 0, 0, 0);

                    LOG(INFO) << "STEAM INACTIVE";
                }
                return 0;
            }
        }

        // send chunk
        uint8_t *buffer = picoquic_provide_stream_data_buffer(bytes, (size == -1) ? 0 : size, (size == -1) ? 1 : 0, (size == -1) ? 0 : 1);
        if (buffer != nullptr) {
            // write chunk data
            memcpy(buffer, data, (size == -1) ? 0 : size);

            if (_backlog > 100000) {
                THREADPOOL.push_task(&Flow::recv_from_connection, this);
            }

            _backlog -= (size == -1) ? 0 : size;
        }

        LOG(INFO) << "SEND IN " << size << " bytes";

        if (size == -1) {
            if (_closed) {
                THREADPOOL.push_task(&quic::FlowContext::delete_flow, (quic::FlowContext*) _context, _stream_id);
            } else {
                LOG(INFO) << "MARKED closed";

                _closed = true;
            }
        }

        return ret;
    }

    int Flow::stream_data(uint8_t *bytes, size_t length) {
        // push to tx queue
        size_t bytes_written = 0;
        while(bytes_written < length) {
            int ret = 0;
            if ((ret = write(_tx_pipe[1], bytes, length)) == -1) {
                LOG(INFO) << "stream_data write errno=" << errno;
                if (errno == EAGAIN) {
                    continue;
                }

                return 0;
            }
            bytes_written += ret;
            _tx_size += ret;
        }

        LOG(INFO) << "RECV OUT " << length << " bytes";

        return 0;
    }

    int Flow::stream_fin(uint8_t *bytes, size_t length) {
        LOG(INFO) << "FIN received";

        //stream_data(bytes, length);
        assert(length == 0);
        close(_tx_pipe[1]);
        close(_tx_pipe[0]);

        return 0;
    }

    std::string Flow::to_string() {
        return "Flow[stream_id=" + std::to_string(_stream_id) + "]";
    }

    Flow::~Flow() {
        // delete connection
        delete _connection;

        VLOG(1) << "~" << to_string();
        LOG(INFO) << "Delete Flow with ID=" << _stream_id;
    }

} // flow