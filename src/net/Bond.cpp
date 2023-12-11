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
#include "../context/Context.h"

namespace net {
    Bond::Bond(net::ipv4::TcpConnection *terConnection, net::ipv4::TcpConnection *satConnection, context::Context *context) : terConnection_(terConnection), satConnection_(satConnection),
    context_(context) {
        //DLOG(INFO) << "Bond(ter=" << terConnection->ToString() << ", sat=" << satConnection->ToString() << ") * " << ToString();

        // start looper for bond connections, start heartbeat
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

    // read packets from bond connections (ter, sat)
    void Bond::RecvFromConnection(net::ipv4::TcpConnection *connection) {
        // create buffer
        packet::Buffer *buffer = packet::Buffer::make(1024 + sizeof(packet::FlowHeader));
        packet::FlowPacket *flowPacket = nullptr;
        int bytes_read = 0;


        // read Packet from Connection
        try {
            // read Header
            unsigned char *ptr = buffer->data();
            while (bytes_read < sizeof(packet::Header)) {
                try {
                    bytes_read += connection->Recv(ptr + bytes_read, sizeof(packet::Header) - bytes_read, 0);
                    // LOG(INFO) << connection->ToString() << " -> READ " << bytes_read << "bytes";
                } catch (Exception e) {
                    LOG(INFO) << e.what();
                }

                // exit if socket closed
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
            }

            assert(bytes_read == sizeof(packet::Header));

            // downcast to packet
            packet::Packet *packet = (packet::Packet *) buffer;

            // skip if HeartBeatHeader
            if (packet->header()->type() == packet::TYPE::HEARTBEAT) {
                //DLOG(INFO) << connection->ToString() << " -> " << ((packet::HeartBeatPacket *)packet)->ToString();
                delete buffer;

                return;
            }

            // otherwise it is a FlowPacket
            assert(packet->header()->type() == packet::TYPE::FLOW);

            // read FlowHeader
            bytes_read = 0;
            ptr = (unsigned char *)packet->header();
            while (bytes_read < (sizeof(packet::FlowHeader) - sizeof(packet::Header))) {
                try {
                    bytes_read += connection->Recv(ptr + sizeof(packet::Header) + bytes_read,
                                                   (sizeof(packet::FlowHeader) - sizeof(packet::Header)) - bytes_read, 0);
                    // LOG(INFO) << connection->ToString() << " -> READ " << bytes_read << "bytes";
                } catch (Exception e) {
                    LOG(INFO) << e.what();
                }

                // exit if socket closed
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
            }
            assert(bytes_read == (sizeof(packet::FlowHeader) - sizeof(packet::Header)));

            // downcast to FlowPacket
            flowPacket = (packet::FlowPacket *) packet;

            // read FlowPacket data from socket
            bytes_read = 0;
            ptr = flowPacket->data();
            uint16_t headerSize = flowPacket->header()->size();
            while (bytes_read < headerSize) {
                try {
                    bytes_read += connection->Recv(ptr + bytes_read,
                                                   headerSize - bytes_read,
                                                   0);
                    // LOG(INFO) << connection->ToString() << " -> READ " << bytes_read << "bytes";
                } catch (Exception e) {
                    LOG(INFO) << e.what();
                }

                // exit if socket closed
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

        // resize FlowPacket to fit data
        flowPacket->Resize(flowPacket->header()->size());

        // log
        //DLOG(ERROR) << connection->ToString() << " -> " + flowPacket->ToString();

        // write FlowPacket to corresponding Flow
        net::Flow *flow = nullptr;
        {
            // sync FlowMap here
            std::lock_guard lock(context_->flows()->mutex());

            // check if flow already exists
            if (!(flow = context_->flows()->Find(flowPacket->header()->source(), flowPacket->header()->destination()))) {
                // create flow if not exists
                if (flowPacket->header()->size() == 0) {
                    return;
                }

                try {
                    // connect to endpoint
                    net::ipv4::TcpConnection *flowConnection = net::ipv4::TcpConnection::make(flowPacket->header()->destination());

                    // create flow
                    flow = net::Flow::make(flowPacket->header()->source(), flowPacket->header()->destination(), flowConnection, this, context_);

                    // insert into FlowMap
                    context_->flows()->Insert(flowPacket->header()->source(), flowPacket->header()->destination(), flow);
                } catch (Exception e) {
                    LOG(INFO) << e.what();

                    // send close packet
                    flowPacket->header()->size(0);

                    SendToTer(flowPacket);

                    return;
                }
            }
        }

        // write FlowPacket to Flow
        flow->WriteToFlow(flowPacket);
    }

    Bond::~Bond() {
        DLOG(INFO) << ToString() << ".~Bond()";

        // stop loopers
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

    // try to write HeartBeatPacket every 100ms
    void Bond::WriteHeartBeatPacket() {
        // if BufferFillLevels are empty write HeartBeatPacket
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

    // write packets (FlowPacket, HeartBeatPacket) to socket
    void Bond::SendToConnection(net::ipv4::TcpConnection *connection, packet::Packet *packet) {
        // upcast to buffer
        packet::Buffer *buffer = packet;

        int bytes_written = 0;
        try {
            while (bytes_written < buffer->size()) {
                //int ret = 0;
                try {
                    bytes_written += connection->Send(buffer->data() + bytes_written, buffer->size() - bytes_written,
                                                      0);
                    // LOG(INFO) << "Write " << bytes_written << "bytes -> " << connection->ToString();
                } catch (Exception e) {
                    continue;
                }

                //assert(ret > 0);

                //bytes_written += ret;

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
    }

    // try to send on ter
    void Bond::SendToTer(packet::FlowPacket *flowPacket) {
        // redirect to sat if not enabled
        if(!args::TER_ENABLED) {
            SendToSat(flowPacket);
            return;
        }

        // wait if BufferFillLevel = BDP + 20%
        while(terConnection_->IoCtl<int>(TIOCOUTQ) > 7500) { // BDP + 20% = 7500 + 1500 = 9000 bytes
            usleep(4219); // 2MBit/s / 8 = 0.25 MB/s = 250000byte/s / 1056 = ~237 packet/s = 1 packet every 4.219 ms = 4129 us
        }

        // send to socket
        sendTerMutex_.lock();
        SendToConnection(terConnection_, flowPacket);
        sendTerMutex_.unlock();

        //DLOG(ERROR) << flowPacket->ToString() << " -> " << terConnection_->ToString();

        // data sent -> delete FLowPacket
        delete flowPacket;
    }

    void Bond::SendToSat(packet::FlowPacket *flowPacket) {
        // redirect to ter if not enabled
        if(!args::SAT_ENABLED) {
            SendToTer(flowPacket);
            return;
        }

        while(satConnection_->IoCtl<int>(TIOCOUTQ) > 3750000) { // 3750000
            // bandwidth aggregation
            /*if(args::TER_ENABLED) {
                if (terConnection_->IoCtl<int>(TIOCOUTQ) < 9000) {
                    SendToTer(flowPacket);
                    return;
                }
            }*/

            std::this_thread::sleep_for(std::chrono::microseconds(168)); // 50MBit/s / 8 = 6.25 MB/s = 6250000byte/s / 1056 = ~5919 packet/s = 1 packet every 0.168 ms = 168 us
        }

        // send to socket
        sendSatMutex_.lock();
        SendToConnection(satConnection_, flowPacket);
        sendSatMutex_.unlock();

        //DLOG(ERROR) << flowPacket->ToString() << " -> " << satConnection_->ToString();

        // data sent -> delete FlowPacket
        delete flowPacket;
    }

} // net