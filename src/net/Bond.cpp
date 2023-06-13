//
// Created by Matthias Hofstätter on 18.04.23.
//

#include <cassert>
#include <functional>
#include <future>
#include <fstream>

#include "Bond.h"

#include "../packet/FlowPacket.h"
#include "../packet/FlowHeader.h"
#include "base/TcpConnection.h"
#include "Flow.h"
#include "../collections/FlowMap.h"
#include "../packet/HeartBeatPacket.h"
#include "../task/ThreadPool.h"
#include "../context/Context.h"

namespace net {
    Bond::Bond(net::ipv4::TcpConnection *terConnection, net::ipv4::TcpConnection *satConnection, context::Context *context) : terConnection_(terConnection), satConnection_(satConnection),
    readFromTerLooper_(worker::Looper(std::bind(&Bond::RecvFromConnection, this, terConnection))),
    readFromSatLooper_(worker::Looper(std::bind(&Bond::RecvFromConnection, this, satConnection))),
                                                                                                                              heartbeatLooper_(worker::Looper(std::bind(&Bond::WriteHeartBeatPacket, this))),
    context_(context) {
        DLOG(INFO) << "Bond(ter=" << terConnection->ToString() << ", sat=" << satConnection->ToString() << ") * " << ToString();
    }

    void Bond::RecvFromConnection(net::ipv4::TcpConnection *connection) {
        // create buffer
        packet::Buffer *buffer = packet::Buffer::make(1024 + sizeof(packet::FlowHeader));
        packet::FlowPacket *flowPacket = nullptr;
        int bytes_read = 0;

        // read Packet from Connection
        try {
            //std::lock_guard lock(connection->recvLock());

            // read Header
            while (bytes_read < sizeof(packet::Header)) {
                bytes_read += connection->Recv(buffer->data() + bytes_read, sizeof(packet::Header) - bytes_read, 0);

                if (bytes_read == 0) {
                    delete buffer;

                    return;
                }

                //DLOG(INFO) << connection->ToString() << " -> READ " << bytes_read << "bytes";
            }
            assert(bytes_read == sizeof(packet::Header));

            packet::Packet *packet = (packet::Packet *) buffer;

            // skip if HeartBeatHeader
            if (packet->header()->type() == packet::TYPE::HEARTBEAT) {
                //DLOG(INFO) << connection->ToString() << " -> " << ((packet::HeartBeatPacket *)packet)->ToString();
                delete buffer;

                // metrics
                context_->metrics()->getConnection(connection->fd())->AddRecvBytes(bytes_read);

                return;
            }
            assert(packet->header()->type() == packet::TYPE::FLOW);

            // read FlowHeader
            bytes_read = 0;
            while (bytes_read < (sizeof(packet::FlowHeader) - sizeof(packet::Header))) {
                bytes_read += connection->Recv((unsigned char *) packet->header() + sizeof(packet::Header) + bytes_read,
                                               (sizeof(packet::FlowHeader) - sizeof(packet::Header)) - bytes_read, 0);

                if (bytes_read == 0) {
                    delete buffer;

                    terConnection_->Shutdown(SHUT_RDWR);
                    satConnection_->Shutdown(SHUT_RDWR);
                    readFromTerLooper_.Stop();
                    readFromSatLooper_.Stop();
                    heartbeatLooper_.Stop();
                    std::thread([this] {
                        delete this;
                    }).detach();

                    return;
                }

                //DLOG(INFO) << connection->ToString() << " -> READ " << bytes_read << "bytes";
            }
            assert(bytes_read == (sizeof(packet::FlowHeader) - sizeof(packet::Header)));

            flowPacket = (packet::FlowPacket *) packet;

            // read packet data from socket
            bytes_read = 0;
            while (bytes_read < flowPacket->header()->size()) {
                bytes_read += connection->Recv(flowPacket->data() + bytes_read,
                                               flowPacket->header()->size() - bytes_read,
                                               0);

                if (bytes_read == 0) {
                    delete buffer;

                    terConnection_->Shutdown(SHUT_RDWR);
                    satConnection_->Shutdown(SHUT_RDWR);
                    readFromTerLooper_.Stop();
                    readFromSatLooper_.Stop();
                    heartbeatLooper_.Stop();
                    std::thread([this] {
                        delete this;
                    }).detach();

                    return;
                }

                //DLOG(INFO) << connection->ToString() << " -> READ " << bytes_read << "recvBytes";
            }
            assert(bytes_read == flowPacket->header()->size());
        } catch (Exception e) {
            // something goes wrong -> quit?!
            LOG(ERROR) << e.what();

            terConnection_->Shutdown(SHUT_RDWR);
            satConnection_->Shutdown(SHUT_RDWR);
            readFromTerLooper_.Stop();
            readFromSatLooper_.Stop();
            heartbeatLooper_.Stop();
            std::thread([this] {
                delete this;
            }).detach();
        }

        // process FlowPacket
        flowPacket->Resize(flowPacket->header()->size());

        //DLOG(INFO) << connection->ToString() << " -> " + flowPacket->ToString();

        // expect more data to read?
        /*if (flowPacket->header()->size() == 1024) {
            // read on
        } else {
            //
            std::thread(&Bond::WaitForData, this, connection).detach();
        }*/

        // write FlowPacket to corresponding Flow
        net::Flow *flow = nullptr;
        {
            std::lock_guard lock(context_->flows()->mutex());

            // check if flow already exists
            if (!(flow = context_->flows()->Find(flowPacket->header()->source(), flowPacket->header()->destination()))) {
                // create flow
                try {
                    net::ipv4::TcpConnection *flowConnection = net::ipv4::TcpConnection::make(flowPacket->header()->destination());

                    // metrics
                    context_->metrics()->addConnection(flowConnection->fd());

                    flow = net::Flow::make(flowPacket->header()->source(), flowPacket->header()->destination(), flowConnection, this, context_);

                    context_->flows()->Insert(flowPacket->header()->source(), flowPacket->header()->destination(), flow);
                } catch (Exception e) {
                    LOG(INFO) << e.what();
                }
            }
        }

        // metrics
        context_->metrics()->getConnection(connection->fd())->AddRecvBytes(bytes_read);

        // find flow
        flow->WriteToFlow(flowPacket);
    }

    Bond::~Bond() {
        DLOG(INFO) << ToString() << ".~Bond()";
    }

    std::string Bond::ToString() {
        return "Bond[ter=" + terConnection_->ToString() + ", sat=" + satConnection_->ToString() + "]";
    }

    void Bond::WriteHeartBeatPacket() {
        packet::HeartBeatPacket *heartBeatPacket = packet::HeartBeatPacket::make();

        //DLOG(INFO) << heartBeatPacket->ToString() << " -> " << satConnection_->ToString();

        sendSatMutex_.lock();
        SendToConnection(satConnection_, heartBeatPacket);
        sendSatMutex_.unlock();

        delete heartBeatPacket;

        usleep(100000); // 100ms
    }

    void Bond::SendToConnection(net::ipv4::TcpConnection *connection, packet::Packet *packet) {
        packet::Buffer *buffer = packet;

        try {
            int bytes_written = 0;
            while (bytes_written < buffer->size()) {
                bytes_written += connection->Send(buffer->data() + bytes_written, buffer->size() - bytes_written,0);

                //DLOG(INFO) << "Write " << bytes_written << "bytes -> " << connection->ToString();
            }

            context_->metrics()->getConnection(connection->fd())->AddSendBytes(bytes_written);

            assert(bytes_written == buffer->size());
        } catch (Exception e) {
            LOG(INFO) << e.what();

            delete buffer;

            terConnection_->Shutdown(SHUT_RDWR);
            satConnection_->Shutdown(SHUT_RDWR);
            readFromTerLooper_.Stop();
            readFromSatLooper_.Stop();
            heartbeatLooper_.Stop();
            std::thread([this] {
                delete this;
            }).detach();

            return;
        }
    }

    void Bond::SendToTer(packet::FlowPacket *flowPacket) {
        //DLOG(INFO) << flowPacket->ToString() << " -> " << terConnection_->ToString();

        //LOG(INFO) << "ter queue: " << satConnection_->IoCtl<int>(TIOCOUTQ);
        while(terConnection_->IoCtl<int>(TIOCOUTQ) > 10000) {
            usleep(5);
        }

        sendTerMutex_.lock();
        context_->flows()->subByteSize(flowPacket->header()->size());
        SendToConnection(terConnection_, flowPacket);
        sendTerMutex_.unlock();

        delete flowPacket;
    }

    void Bond::SentToSat(packet::FlowPacket *flowPacket) {
        //DLOG(INFO) << flowPacket->ToString() << " -> " << satConnection_->ToString();

        //LOG(INFO) << "sat queue: " << satConnection_->IoCtl<int>(TIOCOUTQ);
        while(satConnection_->IoCtl<int>(TIOCOUTQ) > 3750000) {
            usleep(5);
        }

        sendSatMutex_.lock();
        context_->flows()->subByteSize(flowPacket->header()->size());
        SendToConnection(satConnection_, flowPacket);
        sendSatMutex_.unlock();

        delete flowPacket;
    }

} // net