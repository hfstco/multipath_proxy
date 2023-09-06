//
// Created by Matthias Hofstätter on 14.04.23.
//

#include <thread>
#include <cassert>
#include <future>

#include "Flow.h"

#include "../net/TcpConnection.h"
#include "../quic/Stream.h"
#include "../quic/FlowContext.h"
#include "picoquic_internal.h"
#include "../backlog/Chunk.h"
#include "../backlog/TotalBacklog.h"


namespace flow {

    Flow::Flow(quic::FlowContext *context, uint64_t stream_id, net::ipv4::SockAddr_In source, net::ipv4::SockAddr_In destination, net::ipv4::TcpConnection *tcp_connection) :   Stream((quic::Context *)context, stream_id),
                                                                                                                                                                                _source(source), // source of Flow
                                                                                                                                                                                _destination(destination), // destination of Flow
                                                                                                                                                                                _connection(tcp_connection), // connection to endpoint (local -> source, remote -> destination)
                                                                                                                                                                                _tx_buffer(nullptr),
                                                                                                                                                                                _tx_buffer_offset(0),
                                                                                                                                                                                _recv_from_connection_looper([this] { recv_from_connection(); }) { // loop for recv_from_connection() (own thread)
        LOG(INFO) << "*" << to_string();

        // start flow workers
        _recv_from_connection_looper.start();
    }

    void Flow::recv_from_connection() {
        // wait for new data
        _connection->Poll(POLLIN | POLLHUP | POLLRDHUP, -1);

        std::lock_guard lock(_recving);

        // create chunk
        auto *chunk = new backlog::Chunk(1024);

        // read available data
        ssize_t bytes_read = 0;
        try {
            bytes_read = _connection->Recv(chunk->bytes(), 1024, 0);
        } catch (Exception &e) {
            LOG(INFO) << e.what();
        }

        chunk->length(bytes_read);

        // socket closed
        if (bytes_read == 0) {
            // stop looper
            _recv_from_connection_looper.stop();

            LOG(INFO) << "SOCKET CLOSED!";
        }

        //LOG(INFO) << _connection->to_string() << " -> " << chunk->to_string();

        // add packet to backlog
        backlog.insert(chunk);

        // mark stream active if inactive
        bool expected = false;
        if (_active.compare_exchange_strong(expected, true)) {
            LOG(INFO) << "STREAM ACTIVE CHANGED: TRUE";
            mark_active_stream(1, this);
        }
    }

    std::string Flow::to_string() {
        return "Flow[source=" + _source.to_string() + ", destination=" + _destination.to_string() + ", stream_id=" + std::to_string(_stream_id) + "]";
    }

    Flow::~Flow() {
        // close and delete connection
        _connection->Close();
        delete _connection;

        LOG(INFO) << "~" << to_string();
    }

    int Flow::prepare_to_send(uint8_t *bytes, size_t length) {
        // update path affinity
        if ((backlog <= 2000 || backlog::total_backlog <= 74219)) { // && std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - flow->last_satellite_timestamp.load()).count() > 600) {
            uint64_t path = 1;
            if (_path.compare_exchange_strong(path, 0)) {
                set_stream_path_affinity(0); // TER

                LOG(INFO) << "SWITCH TO TER";
            }
        } else {
            // TODO is available?
            uint64_t path = 0;
            if (_path.compare_exchange_strong(path, 1)) {
                set_stream_path_affinity(1); // SAT

                LOG(INFO) << "SWITCH TO SAT";
            }
        }

        // get next chunk
        if (_tx_buffer_offset == 0) {
            _tx_buffer = backlog.next();

            {
                // sync with recv
                std::lock_guard lock(_recving);

                if (_tx_buffer == nullptr) {
                    // no data available, mark flow/stream inactive
                    bool expected = true;
                    if (_active.compare_exchange_strong(expected, false)) {
                        picoquic_provide_stream_data_buffer(bytes, 0, 0, 0);

                        LOG(INFO) << "STEAM INACTIVE";

                        return 0;
                    }
                }
            }
        }

        uint64_t to_send = std::min(_tx_buffer->length() - _tx_buffer_offset, length);

        // send chunk
        uint8_t *buffer = picoquic_provide_stream_data_buffer(bytes, to_send, (_tx_buffer->length() == 0 ) ? 1 : 0, (_tx_buffer->length() == 0 ) ? 0 : 1);
        if (buffer != nullptr) {
            // write chunk data
            memcpy(buffer, _tx_buffer->bytes() + _tx_buffer_offset, to_send);
        }

        _tx_buffer_offset += to_send;
        if (_tx_buffer_offset == _tx_buffer->length()) {
            // reset buffer
            _tx_buffer = nullptr;
            _tx_buffer_offset = 0;
        }

        return 0;
    }

    int Flow::stream_data(uint8_t *bytes, size_t length) {
        // TODO loop until send
        _connection->Send(bytes, length, 0);

        return 0;
    }

    int Flow::stream_fin(uint8_t *bytes, size_t length) {
        LOG(INFO) << "FIN received";

        // TODO loop until send
        _connection->Send(bytes, length, 0);
        _connection->Shutdown(SHUT_RDWR);

        return 0;
    }

} // flow