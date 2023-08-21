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
#include "../backlog/ChunkMap.h"
#include "QuicStream.h"


namespace net {

    Flow::Flow(net::ipv4::SockAddr_In source, net::ipv4::SockAddr_In destination, net::ipv4::TcpConnection *tcpConnection) : _source(source), // source of Flow
                                                                                                                             _destination(destination), // destination of Flow
                                                                                                                             _connection(tcpConnection), // connection to endpoint (local -> source, remote -> destination)
                                                                                                                             _rxBacklog(backlog::Backlog()), // incoming backlog
                                                                                                                             _txChunkMap(backlog::ChunkMap()), // outgoing backlog
                                                                                                                             _recvFromConnectionLooper([this] { RecvFromConnection(); }), // loop for RecvFromConnection() (own thread)
                                                                                                                             _sendToConnectionLooper([this] { SendToConnection(); }) { // loop for RecvFromConnection() (own thread)
        LOG(INFO) << "Flow(" << _source.ToString() << "|" << _destination.ToString() << ", " << tcpConnection->ToString() << ")";
    }

    Flow *Flow::make(net::ipv4::SockAddr_In source, net::ipv4::SockAddr_In destination, net::ipv4::TcpConnection *tcpConnection) {
        return new Flow(source, destination, tcpConnection);
    }

    net::ipv4::SockAddr_In Flow::source() {
        return _source;
    }

    net::ipv4::SockAddr_In Flow::destination() {
        return _destination;
    }

    size_t Flow::size() {
        return _rxBacklog.size();
    }

    bool Flow::useSatellite() {
        return _useSatellite.load();
    }

    void Flow::RecvFromConnection() {
        // wait for new data
        _connection->Poll(POLLIN | POLLHUP | POLLRDHUP, -1);

        // preload stop
        if (_rxBacklog.size() >= 4500000) {
            return;
        }

        // read available data
        ssize_t bytes_read = 0;
        auto *buffer = static_cast<unsigned char *>(malloc(1024));
        try {
            bytes_read = _connection->Recv(buffer, 1024, 0);
        } catch (Exception &e) {
            LOG(INFO) << e.what();
        }

        // socket closed
        if (bytes_read == 0) {
            _recvFromConnectionLooper.Stop();
            _closed.store(true);
        }

        // insert chunk into backlog
        auto *chunk = new backlog::Chunk();
        chunk->data = buffer;
        chunk->size = bytes_read;
        chunk->offset = _rxOffset.fetch_add(bytes_read);
        _rxBacklog.Insert(chunk);

        if((_rxBacklog.size() < 2000 || backlog::TotalBacklog::size() < 74219 || chunk->offset == 0) && !_useSatellite.load()) {
            _terTxStream->MarkActiveStream();
        } else {
            _useSatellite.store(true);
            _satTxStream->MarkActiveStream();
        }
    }

    void Flow::SendToConnection() {
        // try to get next package
        backlog::Chunk *chunk = _txChunkMap.Next(1024); // TODO buffer size

        // next packet not available
        if (chunk == nullptr) {
            usleep(100); // TODO blocking queue?
            return;
        }

        // close packet
        /*if (flowPacket->header()->size() == 0) {
            delete flowPacket;

            // TODO
            _sendToConnectionLooper.Stop();
            _closed.test_and_set();
        }*/

        // send packet to connection
        ssize_t bytes_send = 0;
        try {
            while (bytes_send < chunk->size) {
                bytes_send += _connection->Send(chunk->data, chunk->size, 0);
            }
        } catch (SocketErrorException e) {
            // socket closed waiting for close package
            LOG(ERROR) << e.ToString();
        } catch (Exception e) {
            LOG(ERROR) << e.ToString();
        }

        //LOG(INFO) << flowPacket->ToString() << " -> " << _connection->ToString();

        // delete package
        delete chunk;
    }

    Flow::~Flow() {
        // close and delete connection
        _connection->Close();
        delete _connection;

        LOG(INFO) << ToString() << ".~Flow()";
    }

    std::string Flow::ToString() {
        return "Flow[source=" + _source.ToString() + ", destination=" + _destination.ToString() + "]";
    }

    void Flow::MakeActiveFlow(int isActive) {
        assert(_terTxStream && _satTxStream);

        if (isActive) {
            _recvFromConnectionLooper.Start();
            _sendToConnectionLooper.Start();
        } else {
            _recvFromConnectionLooper.Stop();
            _sendToConnectionLooper.Stop();
        }
    }

    backlog::Backlog &Flow::Backlog() {
        return _rxBacklog;
    }

    backlog::Backlog &Flow::rx() {
        return _rxBacklog;
    }

    backlog::ChunkMap &Flow::tx() {
        return _txChunkMap;
    }

} // net