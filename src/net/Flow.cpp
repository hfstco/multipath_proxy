//
// Created by Matthias Hofstätter on 14.04.23.
//

#include <thread>
#include <assert.h>
#include <future>

#include "Flow.h"

#include "TcpConnection.h"
#include "../packet/FlowPacket.h"
#include "../packet/FlowHeader.h"
#include "Bond.h"
#include "../collections/FlowMap.h"
#include "../task/ThreadPool.h"
#include "../context/Context.h"
#include "../packet/PicoPacket.h"

namespace net {

    Flow::Flow(net::ipv4::SockAddr_In source, net::ipv4::SockAddr_In destination, net::ipv4::TcpConnection *tcpConnection, net::Bond *bond, context::Context *context) : source_(source),
                                                                                                                                                                         destination_(destination),
                                                                                                                                                                         connection_(tcpConnection),
                                                                                                                                                                         bond_(bond),
                                                                                                                                                                         toBondId_(0),
                                                                                                                                                                         toConnectionQueue_(collections::SortedBacklog()),
                                                                                                                                                                         toBondQueue_(collections::UnsortedBacklog()),
                                                                                                                                                                         context_(context),
                                                                                                                                                                         recvFromConnectionLooper_(std::bind(&Flow::RecvFromConnection, this)),
                                                                                                                                                                         sendToConnectionLooper_(std::bind(&Flow::SendToConnection, this)),
                                                                                                                                                                         sendToBondLooper_(std::bind(&Flow::SendToBond, this))
                                                                                                                                                                         {
        LOG(ERROR) << "Flow(" << source_.to_string() << "|" << destination_.to_string() << ", " << tcpConnection->ToString() << ")";

        recvFromConnectionLooper_.Start();
        sendToConnectionLooper_.Start();
        sendToBondLooper_.Start();
    }

    Flow *Flow::make(net::ipv4::SockAddr_In source, net::ipv4::SockAddr_In destination, net::ipv4::TcpConnection *pTcpConnection, net::Bond *bond, context::Context *context) {
        return new Flow(source, destination, pTcpConnection, bond, context);
    }

    //void Flow::WriteToFlow(packet::FlowPacket *pFlowPacket) {
    void Flow::WriteToFlow(packet::PicoPacket *pico_packet) {
        //toConnectionQueue_.push(pFlowPacket);
        toConnectionQueue_.push(pico_packet);
    }

    void Flow::RecvFromConnection() {
        connection_->Poll(POLLIN | POLLRDHUP | POLLHUP, -1);

        // preload stop
        if (size() >= 4500000) {
            usleep(168);
            return;
        }

        // create packet
        //packet::FlowHeader flowHeader = packet::FlowHeader(source_, destination_, toBondId_++); // TODO GetSock/PeerName local variable?
        //packet::FlowPacket *flowPacket = packet::FlowPacket::make(flowHeader, 1024); // TODO BUFFER_SIZE;
        packet::PicoPacket *pico_packet = new packet::PicoPacket;
        pico_packet->source_ip = source_.sin_addr;
        pico_packet->source_port = source_.sin_port;
        pico_packet->destination_ip = destination_.sin_addr;
        pico_packet->destination_port = destination_.sin_port;
        pico_packet->id = toBondId_++;

        // read data from connection
        ssize_t bytes_read = 0;
        try {
            //bytes_read = connection_->Recv(flowPacket->data(), flowPacket->header()->size(), 0);
            bytes_read = connection_->Recv(pico_packet->payload, pico_packet->size, 0);
            //LOG(INFO) << connection_->ToString() << " -> READ " << bytes_read << "bytes";
        } catch (Exception e) {
             LOG(INFO) << e.what();
        }

        // socket closed
        if (bytes_read == 0) {
            recvFromConnectionLooper_.Stop();

            if(closed_) {
                toBondQueue_.reset();
                assert(toBondQueue_.empty());
                //flowPacket->header()->id(toBondQueue_.currentId());
                pico_packet->id = toBondQueue_.currentId();
            }
        }

        // resize packet to fit data
        //flowPacket->Resize(bytes_read);
        pico_packet->size = bytes_read;

        //DLOG(INFO) << connection_->ToString() << " -> " << flowPacket->ToString();
        DLOG(INFO) << connection_->ToString() << " -> " << pico_packet->to_string();

        // write package to toBond_ queue
        //toBondQueue_.push(flowPacket);
        toBondQueue_.push(pico_packet);
        context_->flows()->addByteSize(bytes_read);
    }

    void Flow::SendToConnection() {
        // try to get next package
        //packet::FlowPacket *flowPacket = toConnectionQueue_.pop();
        packet::PicoPacket *pico_packet = toConnectionQueue_.pop();

        // close packet
        //if (flowPacket->header()->size() == 0) {
        if (pico_packet->size == 0) {
            //delete flowPacket;
            delete pico_packet;

            //assert(toConnectionQueue_.Empty());

            sendToConnectionLooper_.Stop();

            if (closed_) {
                assert(!recvFromConnectionLooper_.IsRunning() && !sendToConnectionLooper_.IsRunning() && !sendToBondLooper_.IsRunning());
                std::thread([this] {
                    delete this;
                }).detach();
            } else {
                closed_ = true;
                connection_->Shutdown(SHUT_RDWR);
            }

            return;
        }

        // send packet to connection
        int bytes_sent = 0;
        try {
            //bytes_sent = connection_->Send(flowPacket->data(), flowPacket->header()->size(), 0);
            bytes_sent = connection_->Send(pico_packet->payload, pico_packet->size, 0);
            //LOG(INFO) << "SENT " << bytes_sent << "bytes -> " << connection_->ToString();
        } catch (Exception e) {
            //LOG(ERROR) << e.ToString();
            // socket closed, skipping packets until close packet arrive
        }

        //DLOG(INFO) << flowPacket->ToString() << " -> " << connection_->ToString();
        DLOG(INFO) << pico_packet->to_string() << " -> " << connection_->ToString();

        // delete package
        //delete flowPacket;
        delete pico_packet;

        // metrics
        /*try {
            context_->metrics()->getConnection(connection_->fd())->AddSendBytes(bytes_sent);
        } catch (NotFoundException e) {
            return;
        }*/
    }

    void Flow::SendToBond() {
        // try to get next package
        //packet::FlowPacket *flowPacket = toBondQueue_.pop();
        packet::PicoPacket *pico_packet = toBondQueue_.pop();

        // close packet
        //if( flowPacket->header()->size() == 0 ) {
        if (pico_packet->size == 0) {
            sendToBondLooper_.Stop();

            if (closed_) {
                assert(!recvFromConnectionLooper_.IsRunning() && !sendToConnectionLooper_.IsRunning() && !sendToBondLooper_.IsRunning());

                std::thread([this] {
                    delete this;
                }).detach();
            } else {
                closed_ = true;
            }

            //assert(byteSize() < 2000);
            //bond_->SendToTer(flowPacket);
            bond_->SendToTer(pico_packet);
            return;
        }

        //context_->flows()->subByteSize(flowPacket->header()->size());
        context_->flows()->subByteSize(pico_packet->size);

        // write packet
        // LOG(INFO) << "backlog: " << byteSize() << ", totalBacklog: " << context_->flows()->getByteSize();
        // send data to ter if backlog & totalBacklog < S, and satellite_ == false
        if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - lastSendTimestamp_).count() > 600) {
            satellite_.clear();
        }

        //if((size() < 2000 || context_->flows()->getByteSize() < 74219 || flowPacket->header()->id() == 0) && !satellite_.test() && args::TER_ENABLED) {
        if((size() < 2000 || context_->flows()->getByteSize() < 74219 || pico_packet->id == 0) && !satellite_.test() && args::TER_ENABLED) {
            //bond_->SendToTer(flowPacket);
            bond_->SendToTer(pico_packet);
        } else if(args::SAT_ENABLED) {
            satellite_.test_and_set();
            //bond_->SendToSat(flowPacket);
            bond_->SendToSat(pico_packet);
        } else {

        }

        // set last send timestamp
        lastSendTimestamp_ = std::chrono::system_clock::now();
    }

    Flow::~Flow() {
        // remove from flows
        context_->flows()->mutex().lock();
        context_->flows()->Erase(source_.sin_addr, source_.sin_port, destination_.sin_addr, destination_.sin_port);
        context_->flows()->mutex().unlock();

        // close and delete connection
        connection_->Close();
        delete connection_;

        LOG(INFO) << ToString() << ".~Flow()";
    }

    uint64_t Flow::size() {
        return toBondQueue_.size();
    }

    std::string Flow::ToString() {
        return "Flow[source=" + source_.to_string() + ", destination=" + destination_.to_string() + "]";
    }

} // net