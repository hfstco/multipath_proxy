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
#include "../backlog/Backlog.h"
#include "QuicStream.h"


namespace net {

    Flow::Flow(net::ipv4::SockAddr_In source, net::ipv4::SockAddr_In destination, net::ipv4::TcpConnection *tcpConnection) : _source(source), // source of Flow
                                                                                                                             _destination(destination), // destination of Flow
                                                                                                                             _connection(tcpConnection), // connection to endpoint (local -> source, remote -> destination)
                                                                                                                             _rxBacklog(backlog::Backlog()), // incoming backlog
                                                                                                                             _txBacklog(backlog::Backlog(false)), // outgoing backlog
                                                                                                                             _recvFromConnectionLooper([this] { RecvFromConnection(); }), // loop for RecvFromConnection() (own thread)
                                                                                                                             _sendToConnectionLooper([this] { SendToConnection(); }) { // loop for RecvFromConnection() (own thread)
        LOG(INFO) << "Flow(" << _source.ToString() << "|" << _destination.ToString() << ", " << tcpConnection->ToString() << ")";

        _recvFromConnectionLooper.Start();
        _sendToConnectionLooper.Start();
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
        return _useSatellite.test();
    }

    void Flow::RecvFromConnection() {
        // preload stop
        if (_rxBacklog.size() >= 4500000) {
            return;
        }

        // read available data
        ssize_t bytes_read = 0;
        unsigned char *buffer = static_cast<unsigned char *>(malloc(1024));
        try {
            bytes_read = _connection->Recv(buffer, 1024, 0);
            //LOG(INFO) << _connection->ToString() << " -> READ " << bytes_read << "bytes";
        } catch (Exception e) {
            LOG(INFO) << e.what();
        }

        // socket closed
        if (bytes_read == 0) {
            _recvFromConnectionLooper.Stop();
            _closed.test_and_set();
        }

        //LOG(INFO) << _connection->ToString() << " -> " << flowPacket->ToString();

        // insert package to toBond_ queue
        backlog::Chunk *chunk = new backlog::Chunk(buffer, bytes_read, _rxOffset.fetch_add(bytes_read));
        _rxBacklog.Insert(chunk);


    }

    void Flow::SendToConnection() {
        // try to get next package
        backlog::Chunk *chunk = _txBacklog.Next(1024); // TODO buffer size

        // next packet not available
        if (chunk == nullptr) {
            usleep(10); // TODO blocking queue?
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

    void Flow::Insert(backlog::Chunk *chunk) {
        _txBacklog.Insert(chunk);
    }

    backlog::Chunk *Flow::Next(uint64_t max) {
        return _rxBacklog.Next(max);
    }

    backlog::Backlog &Flow::Backlog() {
        return _rxBacklog;
    }

} // net