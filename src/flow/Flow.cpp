//
// Created by Matthias Hofstätter on 14.04.23.
//

#include <thread>
#include <cassert>
#include <future>

#include "Flow.h"

#include "../net/TcpConnection.h"
#include "../backlog/SortedBacklog.h"
#include "../quic/Stream.h"
#include "../quic/FlowContext.h"
#include "picoquic_internal.h"
#include "../backlog/Chunk.h"


namespace flow {

    Flow::Flow(quic::FlowContext *context, uint64_t stream_id, net::ipv4::SockAddr_In source, net::ipv4::SockAddr_In destination, net::ipv4::TcpConnection *tcp_connection) :   Stream((quic::Context *)context, stream_id),
                                                                                                                                                                                _context(context),
                                                                                                                                                                                _source(source), // source of Flow
                                                                                                                                                                                _destination(destination), // destination of Flow
                                                                                                                                                                                _connection(tcp_connection), // connection to endpoint (local -> source, remote -> destination)
                                                                                                                                                                                _tx_backlog(backlog::SortedBacklog()), // outgoing backlog
                                                                                                                                                                                _recv_from_connection_looper([this] { recv_from_connection(); }), // loop for recv_from_connection() (own thread)
                                                                                                                                                                                _send_to_connection_looper([this] { send_to_connection(); }) { // loop for recv_from_connection() (own thread)
        LOG(INFO) << "*" << to_string();

        // start flow workers
        _recv_from_connection_looper.start();
        _send_to_connection_looper.start();
    }

    net::ipv4::SockAddr_In Flow::source() {
        return _source;
    }

    net::ipv4::SockAddr_In Flow::destination() {
        return _destination;
    }

    void Flow::recv_from_connection() {
        // wait for new data
        _connection->Poll(POLLIN | POLLHUP | POLLRDHUP, -1);

        // read available data
        ssize_t bytes_read = 0;
        auto* buffer = (uint8_t*)malloc(1024);
        try {
            bytes_read = _connection->Recv(buffer, 1024, 0);
        } catch (Exception &e) {
            LOG(INFO) << e.what();
        }

        // write ChunkHeader to buffer
        backlog::ChunkHeader chunkHeader = {
                .offset = _rx_offset.fetch_add(bytes_read),
                .size = static_cast<uint64_t>(bytes_read)
        };

        auto *chunk = new backlog::Chunk();
        chunk->header = chunkHeader;
        chunk->data = buffer;

        // socket closed
        if (bytes_read == 0) {
            // stop looper
            _recv_from_connection_looper.stop();

            /*if (_closed) {
                // delete this
                assert(!_recv_from_connection_looper.is_running() && !_send_to_connection_looper.is_running());
                std::thread([this] {
                    delete this;
                }).detach();
            }*/

            _closed = true;
        }

        //LOG(INFO) << _connection->to_string() << " -> " << chunk->to_string();

        // add packet to backlog
        backlog.insert(chunk);

        bool expected = false;
        if (_active.compare_exchange_strong(expected, true)) {
            LOG(INFO) << "STREAM ACTIVE CHANGED: TRUE";
            mark_active_stream(1, this);
        }

        //add_to_stream_with_ctx(buffer, bytes_read, 0, this);
    }

    void Flow::send_to_connection() {
        // try to get next package
        backlog::Chunk *chunk = _tx_backlog.next(1500); // TODO buffer size

        // next packet not available
        if (chunk == nullptr) {
            usleep(5); // TODO blocking queue?
            return;
        }

        // close chunk
        if (chunk->header.size == 0) {
            delete chunk;

            // stop looper
            _send_to_connection_looper.stop();

            /*if (_closed) {
                assert(!_recv_from_connection_looper.is_running() && !_send_to_connection_looper.is_running());
                std::thread([this] {
                    delete this;
                }).detach();
            }*/

            _closed = true;
            _connection->Shutdown(SHUT_RDWR);

            return;
        }

        // send packet to connection
        ssize_t bytes_send = 0;
        try {
            while (bytes_send < chunk->header.size) {
                bytes_send += _connection->Send(chunk->data, chunk->header.size, 0);
            }
        } catch (SocketErrorException e) {
            // socket closed waiting for close package
            LOG(ERROR) << e.ToString();
        } catch (Exception e) {
            LOG(ERROR) << e.ToString();
        }

        //LOG(INFO) << chunk->to_string() << " -> " << _connection->to_string();

        // delete package
        delete chunk;
    }

    /*void Flow::make_active(int isActive) {
        assert(_terTxStream && _satTxStream);

        if (isActive) {
            _recv_from_connection_looper.Start();
            _send_to_connection_looper.Start();
        } else {
            _recv_from_connection_looper.Stop();
            _send_to_connection_looper.stop();
        }
    }

    backlog::UnsortedBacklog &Flow::UnsortedBacklog() {
        return _rxBacklog;
    }

    backlog::UnsortedBacklog &Flow::rx() {
        return _rxBacklog;
    }*/

    backlog::SortedBacklog &Flow::tx() {
        return _tx_backlog;
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

    int Flow::callback(picoquic_cnx_t *cnx, uint64_t stream_id, uint8_t *bytes, size_t length,
                       picoquic_call_back_event_t fin_or_event, void *callback_ctx, void *v_stream_ctx) {
        return 0;
    }

} // flow