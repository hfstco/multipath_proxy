//
// Created by Matthias Hofstätter on 18.04.23.
//

#include <cassert>
#include <functional>
#include <future>
#include <fstream>
#include <unistd.h>
#include <fcntl.h>

#include "Bond.h"

#include "../packet/FlowPacket.h"
#include "../packet/FlowHeader.h"
#include "TcpConnection.h"
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

        // performance
        /*std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
        std::chrono::time_point<std::chrono::system_clock> now;
        uint64_t duration;*/

        // read Packet from Connection
        try {
            //std::lock_guard lock(connection->recvLock());

            // read Header
            unsigned char *ptr = buffer->data();
            while (bytes_read < sizeof(packet::Header)) {
                try {
                    bytes_read += connection->Recv(ptr + bytes_read, sizeof(packet::Header) - bytes_read, 0);
                } catch (Exception e) {
                    LOG(INFO) << e.what();
                }

                /*now = std::chrono::system_clock::now();
                duration = std::chrono::duration_cast<std::chrono::microseconds>(now - start).count();
                LOG_IF(INFO, duration > 100) << "RecvFromConnection() read header " << std::to_string(duration) << " microseconds";
                start = now;*/

                //LOG(INFO) << connection->ToString() << " -> READ " << bytes_read << "bytes";
            }

            assert(bytes_read == sizeof(packet::Header));

            packet::Packet *packet = (packet::Packet *) buffer;

            // skip if HeartBeatHeader
            if (packet->header()->type() == packet::TYPE::HEARTBEAT) {
                //DLOG(INFO) << connection->ToString() << " -> " << ((packet::HeartBeatPacket *)packet)->ToString();
                delete buffer;

                // metrics
                //context_->metrics()->getConnection(connection->fd())->AddRecvBytes(bytes_read);

                return;
            }
            assert(packet->header()->type() == packet::TYPE::FLOW);

            // read FlowHeader
            bytes_read = 0;
            ptr = (unsigned char *)packet->header();
            while (bytes_read < (sizeof(packet::FlowHeader) - sizeof(packet::Header))) {
                try {
                    bytes_read += connection->Recv(ptr + sizeof(packet::Header) + bytes_read,
                                                   (sizeof(packet::FlowHeader) - sizeof(packet::Header)) - bytes_read, 0);
                } catch (Exception e) {
                    LOG(INFO) << e.what();
                }

                /*now = std::chrono::system_clock::now();
                duration = std::chrono::duration_cast<std::chrono::microseconds>(now - start).count();
                LOG_IF(INFO, duration > 100) << "RecvFromConnection() read flow header " << std::to_string(duration) << " microseconds";
                start = now;*/

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

                //LOG(INFO) << connection->ToString() << " -> READ " << bytes_read << "bytes";
            }
            assert(bytes_read == (sizeof(packet::FlowHeader) - sizeof(packet::Header)));

            flowPacket = (packet::FlowPacket *) packet;

            /*now = std::chrono::system_clock::now();
            duration = std::chrono::duration_cast<std::chrono::microseconds>(now - start).count();
            LOG_IF(INFO, duration > 100) << "RecvFromConnection() before read data " << std::to_string(duration) << " microseconds";
            start = now;*/

            // read packet data from socket
            bytes_read = 0;
            ptr = flowPacket->data();
            uint16_t headerSize = flowPacket->header()->size();
            while (bytes_read < headerSize) {
                try {
                    bytes_read += connection->Recv(ptr + bytes_read,
                                                   headerSize - bytes_read,
                                                   0);

                } catch (Exception e) {
                    LOG(INFO) << e.what();
                }

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

               // LOG(INFO) << connection->ToString() << " -> READ " << bytes_read << "bytes";
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

        /*now = std::chrono::system_clock::now();
        duration = std::chrono::duration_cast<std::chrono::microseconds>(now - start).count();
        LOG(INFO) << "RecvFromConnection() read " << std::to_string(duration) << " microseconds";
        start = now;*/

        // process FlowPacket
        flowPacket->Resize(flowPacket->header()->size());

        /*now = std::chrono::system_clock::now();
        duration = std::chrono::duration_cast<std::chrono::microseconds>(now - start).count();
        LOG_IF(INFO, duration > 50) << "RecvFromConnection() resize " << std::to_string(duration) << " microseconds";
        start = now;*/

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

            /*now = std::chrono::system_clock::now();
            duration = std::chrono::duration_cast<std::chrono::microseconds>(now - start).count();
            LOG_IF(INFO, duration > 50) << "RecvFromConnection() lock " << std::to_string(duration) << " microseconds";
            start = now;*/

            // check if flow already exists
            if (!(flow = context_->flows()->Find(flowPacket->header()->source(), flowPacket->header()->destination()))) {
                // create flow

                try {
                    net::ipv4::TcpConnection *flowConnection = net::ipv4::TcpConnection::make(flowPacket->header()->destination());

                    // metrics
                    context_->metrics()->addConnection(flowConnection->fd());

                    flow = net::Flow::make(flowPacket->header()->source(), flowPacket->header()->destination(), flowConnection, this, context_);

                    /*now = std::chrono::system_clock::now();
                    duration = std::chrono::duration_cast<std::chrono::microseconds>(now - start).count();
                    LOG_IF(INFO, duration > 50) << "RecvFromConnection() add flow " << std::to_string(duration) << " microseconds";
                    start = now;*/

                    context_->flows()->Insert(flowPacket->header()->source(), flowPacket->header()->destination(), flow);

                    /*now = std::chrono::system_clock::now();
                    duration = std::chrono::duration_cast<std::chrono::microseconds>(now - start).count();
                    LOG_IF(INFO, duration > 50) << "RecvFromConnection() insert flow " << std::to_string(duration) << " microseconds";
                    start = now;*/
                } catch (Exception e) {
                    LOG(INFO) << e.what();
                }
            }
        }

        // metrics
        //context_->metrics()->getConnection(connection->fd())->AddRecvBytes(bytes_read);

        // find flow
        flow->WriteToFlow(flowPacket);

        /*now = std::chrono::system_clock::now();
        duration = std::chrono::duration_cast<std::chrono::microseconds>(now - start).count();
        LOG_IF(INFO, duration > 50) << "RecvFromConnection() write to flow " << std::to_string(duration) << " microseconds";*/
    }

    Bond::~Bond() {
        DLOG(INFO) << ToString() << ".~Bond()";
    }

    std::string Bond::ToString() {
        return "Bond[ter=" + terConnection_->ToString() + ", sat=" + satConnection_->ToString() + "]";
    }

    // try every 100ms to write a HeartBeatPacket
    void Bond::WriteHeartBeatPacket() {
        if(satConnection_->IoCtl<int>(TIOCOUTQ) == 0) { // only if sat send buffer is empty
            if(sendSatMutex_.try_lock()) {
                packet::HeartBeatPacket *heartBeatPacket = packet::HeartBeatPacket::make();
                SendToConnection(satConnection_, heartBeatPacket);
                sendSatMutex_.unlock();
                delete heartBeatPacket;
            }
        }

        usleep(100000); // 100ms
    }

    void Bond::SendToConnection(net::ipv4::TcpConnection *connection, packet::Packet *packet) {
        packet::Buffer *buffer = packet;

        try {
            int bytes_written = 0;
            while (bytes_written < buffer->size()) {
                int ret = 0;
                try {
                    ret = connection->Send(buffer->data() + bytes_written, buffer->size() - bytes_written,
                                                      0);
                } catch (Exception e) {
                    continue;
                }

                assert(ret > 0);

                bytes_written += ret;

                //LOG(INFO) << "Write " << bytes_written << "bytes -> " << connection->ToString();
            }

            //metrics
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
            usleep(10);
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
            usleep(10);
        }

        sendSatMutex_.lock();
        context_->flows()->subByteSize(flowPacket->header()->size());
        SendToConnection(satConnection_, flowPacket);
        sendSatMutex_.unlock();

        delete flowPacket;
    }

} // net