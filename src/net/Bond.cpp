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
    context_(context) {
        //DLOG(INFO) << "Bond(ter=" << terConnection->ToString() << ", sat=" << satConnection->ToString() << ") * " << ToString();

        if(args::SAT_ENABLED) {
            readFromSatLooper_ = new worker::Looper(std::bind(&Bond::RecvFromConnection, this, satConnection));
            readFromSatLooper_->Start();

            if(args::HEARTBEAT_ENABLED) {
                heartbeatLooper_ = new worker::Looper(std::bind(&Bond::WriteHeartBeatPacket, this));
                heartbeatLooper_->Start();
            }
        }
        if(args::TER_ENABLED) {
            readFromTerLooper_ = new worker::Looper(std::bind(&Bond::RecvFromConnection, this, terConnection));
            readFromTerLooper_->Start();
        }
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
            unsigned char *ptr = buffer->data();
            while (bytes_read < sizeof(packet::Header)) {
                try {
                    bytes_read += connection->Recv(ptr + bytes_read, sizeof(packet::Header) - bytes_read, 0);
                } catch (Exception e) {
                    LOG(INFO) << e.what();
                }

                //LOG(INFO) << connection->ToString() << " -> READ " << bytes_read << "bytes";
            }

            assert(bytes_read == sizeof(packet::Header));

            packet::Packet *packet = (packet::Packet *) buffer;

            // skip if HeartBeatHeader
            if (packet->header()->type() == packet::TYPE::HEARTBEAT) {
                //DLOG(INFO) << connection->ToString() << " -> " << ((packet::HeartBeatPacket *)packet)->ToString();
                delete buffer;

                // metrics
                /*try {
                    context_->metrics()->getConnection(connection->fd())->AddRecvBytes(bytes_read);
                } catch (NotFoundException e) {
                    return;
                }*/

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

                if (bytes_read == 0) {
                    delete buffer;

                    terConnection_->Shutdown(SHUT_RDWR);
                    satConnection_->Shutdown(SHUT_RDWR);
                    readFromTerLooper_->Stop();
                    readFromSatLooper_->Stop();
                    heartbeatLooper_->Stop();
                    std::thread([this] {
                        delete this;
                    }).detach();

                    return;
                }

                //LOG(INFO) << connection->ToString() << " -> READ " << bytes_read << "bytes";
            }
            assert(bytes_read == (sizeof(packet::FlowHeader) - sizeof(packet::Header)));

            flowPacket = (packet::FlowPacket *) packet;

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
                    readFromTerLooper_->Stop();
                    readFromSatLooper_->Stop();
                    heartbeatLooper_->Stop();
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
            readFromTerLooper_->Stop();
            readFromSatLooper_->Stop();
            heartbeatLooper_->Stop();
            std::thread([this] {
                delete this;
            }).detach();
        }

        // process FlowPacket
        flowPacket->Resize(flowPacket->header()->size());

        DLOG(INFO) << connection->ToString() << " -> " + flowPacket->ToString();

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
                    //context_->metrics()->addConnection(flowConnection->fd());

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

        // find flow
        flow->WriteToFlow(flowPacket);

        // metrics
        /*try {
            context_->metrics()->getConnection(connection->fd())->AddRecvBytes(bytes_read);
        } catch (NotFoundException e) {
            return;
        }*/
    }

    Bond::~Bond() {
        DLOG(INFO) << ToString() << ".~Bond()";

        if(args::SAT_ENABLED) {
            delete readFromSatLooper_;

            if(args::HEARTBEAT_ENABLED) {
                delete heartbeatLooper_;
            }
        }
        if(args::TER_ENABLED) {
            delete readFromTerLooper_;
        }
    }

    std::string Bond::ToString() {
        return "Bond[ter=" + terConnection_->ToString() + ", sat=" + satConnection_->ToString() + "]";
    }

    // try every 100ms to write a HeartBeatPacket
    void Bond::WriteHeartBeatPacket() {
        if(satConnection_->IoCtl<int>(TIOCOUTQ) == 0 || satConnection_->IoCtl<int>(FIONREAD) == 0) { // only if sat send buffer is empty
            if(sendSatMutex_.try_lock()) {
                packet::HeartBeatPacket *heartBeatPacket = packet::HeartBeatPacket::make();
                SendToConnection(satConnection_, heartBeatPacket);
                sendSatMutex_.unlock();
                delete heartBeatPacket;
            }
        }

        usleep(1000000); // 1s
    }

    void Bond::SendToConnection(net::ipv4::TcpConnection *connection, packet::Packet *packet) {
        packet::Buffer *buffer = packet;

        int bytes_written = 0;
        try {
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

            assert(bytes_written == buffer->size());
        } catch (Exception e) {
            LOG(INFO) << e.what();

            delete buffer;

            terConnection_->Shutdown(SHUT_RDWR);
            satConnection_->Shutdown(SHUT_RDWR);
            readFromTerLooper_->Stop();
            readFromSatLooper_->Stop();
            heartbeatLooper_->Stop();
            std::thread([this] {
                delete this;
            }).detach();

            return;
        }

/*        try {
            //metrics
            context_->metrics()->getConnection(connection->fd())->AddSendBytes(bytes_written);
        } catch (NotFoundException e) {
            return;
        }*/
    }

    void Bond::SendToTer(packet::FlowPacket *flowPacket) {
        if(!args::TER_ENABLED) {
            SentToSat(flowPacket);
            return;
        }

        while(terConnection_->IoCtl<int>(TIOCOUTQ) > 10000) {
            //LOG(INFO) << "Waiting ter...";
            //usleep(4219); // 2MBit/s / 8 = 0.25 MB/s = 250000byte/s / 1056 = ~237 packet/s = 1 packet every 4.219 ms = 4129 us
        }

        sendTerMutex_.lock();
        context_->flows()->subByteSize(flowPacket->header()->size());
        SendToConnection(terConnection_, flowPacket);
        sendTerMutex_.unlock();

        DLOG(INFO) << flowPacket->ToString() << " -> " << terConnection_->ToString();

        delete flowPacket;
    }

    void Bond::SentToSat(packet::FlowPacket *flowPacket) {
        if(!args::SAT_ENABLED) {
            SendToTer(flowPacket);
            return;
        }

        if(satConnection_->IoCtl<int>(TIOCOUTQ) > 4500000) { // 3750000
            //LOG(INFO) << "Waiting sat...";
            if(terConnection_->IoCtl<int>(TIOCOUTQ) == 0) {
                SendToTer(flowPacket);
                return;
            }
            usleep(168); // 50MBit/s / 8 = 6.25 MB/s = 6250000byte/s / 1056 = ~5919 packet/s = 1 packet every 0.168 ms = 168 us
        }

        sendSatMutex_.lock();
        context_->flows()->subByteSize(flowPacket->header()->size());
        SendToConnection(satConnection_, flowPacket);
        sendSatMutex_.unlock();

        DLOG(INFO) << flowPacket->ToString() << " -> " << satConnection_->ToString();

        delete flowPacket;
    }

} // net