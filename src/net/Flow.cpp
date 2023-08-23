//
// Created by Matthias Hofstätter on 14.04.23.
//

#include <thread>
#include <assert.h>
#include <future>

#include "Flow.h"

#include "TcpConnection.h"
#include "../packet/FlowHeader.h"
#include "TER.h"
#include "SAT.h"
#include "QuicConnection.h"
#include "../backlog/SortedBacklog.h"
#include "QuicStream.h"
#include "picoquic_internal.h"
#include "../backlog/Chunk.h"


namespace net {

    Flow::Flow(net::ipv4::SockAddr_In source, net::ipv4::SockAddr_In destination, net::ipv4::TcpConnection *tcp_connection) :   _source(source), // source of Flow
                                                                                                                                _destination(destination), // destination of Flow
                                                                                                                                _connection(tcp_connection), // connection to endpoint (local -> source, remote -> destination)
                                                                                                                                _ter_stream(TER->create_stream(this)),
                                                                                                                                _sat_stream(SAT->create_stream(this)),
                                                                                                                                _tx_backlog(backlog::SortedBacklog()), // outgoing backlog
                                                                                                                                _recv_from_connection_looper([this] { recv_from_connection(); }), // loop for recv_from_connection() (own thread)
                                                                                                                                _send_to_connection_looper([this] { send_to_connection(); }) { // loop for recv_from_connection() (own thread)
        LOG(INFO) << "Flow(" << _source.to_string() << "|" << _destination.to_string() << ", " << tcp_connection->ToString() << ")";

        // start flow workers
        make_active();
    }

    Flow::Flow(net::ipv4::SockAddr_In source, net::ipv4::SockAddr_In destination, net::ipv4::TcpConnection *tcp_connection, uint64_t stream_id) : _source(source), // source of Flow
                                                                                                                                                  _destination(destination), // destination of Flow
                                                                                                                                                  _connection(tcp_connection), // connection to endpoint (local -> source, remote -> destination)
                                                                                                                                                  _ter_stream(TER->create_stream(stream_id, this)),
                                                                                                                                                  _sat_stream(SAT->create_stream(stream_id, this)),
                                                                                                                                                  _tx_backlog(backlog::SortedBacklog()), // outgoing backlog
                                                                                                                                                  _recv_from_connection_looper([this] { recv_from_connection(); }), // loop for recv_from_connection() (own thread)
                                                                                                                                                  _send_to_connection_looper([this] { send_to_connection(); }) { // loop for recv_from_connection() (own thread)
        LOG(INFO) << "Flow(" << _source.to_string() << "|" << _destination.to_string() << ", " << tcp_connection->ToString() << ")";

        // start flow workers
        make_active();
    }

    Flow *Flow::make(net::ipv4::SockAddr_In source, net::ipv4::SockAddr_In destination, net::ipv4::TcpConnection *tcp_connection) {
        /*Flow *flow = new Flow(source, destination, tcp_connection);

        // create streams
        flow->_ter_stream = TER->create_stream(flow);
        flow->_sat_stream = SAT->create_stream(flow);

        // start flow workers
        flow->make_active();

        return flow;*/
        return new Flow(source, destination, tcp_connection);
    }

    Flow *Flow::make(net::ipv4::SockAddr_In source, net::ipv4::SockAddr_In destination, net::ipv4::TcpConnection *tcp_connection, uint64_t stream_id) {
        /*Flow *flow = new Flow(source, destination, tcp_connection);

        // create tx streams
        flow->_ter_stream = TER->create_stream(stream_id, flow);
        flow->_sat_stream = SAT->create_stream(stream_id, flow);

        // start flow workers
        flow->make_active();

        return flow;*/
        return new Flow(source, destination, tcp_connection, stream_id);
    }

    net::ipv4::SockAddr_In Flow::source() {
        return _source;
    }

    net::ipv4::SockAddr_In Flow::destination() {
        return _destination;
    }

    bool Flow::use_satellite() {
        return _use_satellite;
    }

    void Flow::recv_from_connection() {
        // wait for new data
        _connection->Poll(POLLIN | POLLHUP | POLLRDHUP, -1);

        // read available data
        ssize_t bytes_read = 0;
        auto *buffer = static_cast<unsigned char *>(malloc(1024));
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

        LOG(INFO) << _connection->to_string() << " -> " << chunk->to_string();

        // add packet to backlog
        backlog.insert(chunk);
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

        LOG(INFO) << chunk->to_string() << " -> " << _connection->to_string();

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
        return "Flow[source=" + _source.to_string() + ", destination=" + _destination.to_string() + "]";
    }

    Flow::~Flow() {
        // close and delete connection
        _connection->Close();
        delete _connection;

        LOG(INFO) << to_string() << ".~Flow()";
    }

/*    uint64_t Flow::backlog() {
        auto *ter_stream_head = picoquic_find_stream(_ter_stream->quic_cnx(), _ter_stream->id());
        auto *sat_stream_head = picoquic_find_stream(_sat_stream->quic_cnx(), _sat_stream->id());

        uint64_t sent_offset = ter_stream_head->sent_offset + sat_stream_head->sent_offset;
        assert(sent_offset <= _sent_offset);
        uint64_t backlog = _sent_offset - sent_offset;

        LOG(INFO) << "BACKLOG ter_sent_offset=" << ter_stream_head->sent_offset << ", sat_sent_offset=" << sat_stream_head->sent_offset << ", _sent_offset=" << _sent_offset << ", backlog=" << backlog;
        return backlog;
    }*/

    void Flow::make_active(int is_active) {
        _recv_from_connection_looper.start();
        _send_to_connection_looper.start();
    }

} // net