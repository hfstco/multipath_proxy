//
// Created by Matthias Hofstätter on 14.04.23.
//

#include <thread>
#include <assert.h>
#include <future>

#include "Flow.h"

#include "base/TcpConnection.h"
#include "../packet/FlowPacket.h"
#include "../packet/FlowHeader.h"
#include "Bond.h"
#include "../collections/FlowMap.h"
#include "../task/ThreadPool.h"
#include "../context/Context.h"

namespace net {

    Flow::Flow(net::ipv4::SockAddr_In source, net::ipv4::SockAddr_In destination, net::ipv4::TcpConnection *tcpConnection, net::Bond *bond, context::Context *context) : source_(source),
                                                                                                                                                                         destination_(destination),
                                                                                                                                                                         connection_(tcpConnection),
                                                                                                                                                                         bond_(bond),
                                                                                                                                                                         toBondId_(0),
                                                                                                                                                                         toConnectionQueue_(collections::BlockingFlowPacketQueue()),
                                                                                                                                                                         toBondQueue_(collections::BlockingFlowPacketQueue()),
                                                                                                                                                                         context_(context),
                                                                                                                                                                         recvFromConnectionLooper_(std::bind(&Flow::RecvFromConnection, this)),
                                                                                                                                                                         sendToConnectionLooper_(std::bind(&Flow::SendToConnection, this)),
                                                                                                                                                                         sendToBondLooper_(std::bind(&Flow::SendToBond, this)) {
        LOG(INFO) << "Flow(" << source_.ToString() << "|" << destination_.ToString() << ", " << tcpConnection->ToString() << ")";
    }

    Flow *Flow::make(net::ipv4::SockAddr_In source, net::ipv4::SockAddr_In destination, net::ipv4::TcpConnection *pTcpConnection, net::Bond *bond, context::Context *context) {
        return new Flow(source, destination, pTcpConnection, bond, context);
    }

    void Flow::WriteToFlow(packet::FlowPacket *pFlowPacket) {
        toConnectionQueue_.Insert(pFlowPacket);
    }

    void Flow::RecvFromConnection() {
        // create packet
        packet::FlowHeader flowHeader = packet::FlowHeader(source_, destination_, toBondId_++); // TODO GetSock/PeerName local variable?
        packet::FlowPacket *flowPacket = packet::FlowPacket::make(flowHeader, 1024); // TODO BUFFER_SIZE;

        // read data from connection
        ssize_t bytes_read = 0;
        try {
            //std::lock_guard lock(connection_->recvLock());

            bytes_read = connection_->Recv(flowPacket->data(), flowPacket->header()->size(), 0);
            // DLOG(INFO) << connection_->ToString() << " -> READ " << bytes_read << "bytes";
        } catch (Exception e) {
            LOG(INFO) << e.what();
        }

        // socket closed
        if (bytes_read == 0) {
            recvFromConnectionLooper_.Stop();
        }

        // resize packet to fit data
        flowPacket->Resize(bytes_read);

        assert(bytes_read == flowPacket->header()->size());
        assert(bytes_read == flowPacket->size() - sizeof(packet::FlowHeader));

        DLOG(INFO) << connection_->ToString() << " -> " << flowPacket->ToString();

        // update metrics
        //context_->flows()->

        // write package to toBond_ queue
        toBondQueue_.Insert(flowPacket);
        context_->flows()->addByteSize(flowPacket->header()->size());
    }

    void Flow::SendToConnection() {
        // try to get next package
        packet::FlowPacket *flowPacket = toConnectionQueue_.Pop();

        // if next package is not available try again
        /*if (!flowPacket) {
            usleep(40);
            return;
        }*/

        // close packet
        if (flowPacket->header()->size() == 0) {
            delete flowPacket;

            sendToConnectionLooper_.Stop();

            if (closed_) {
                std::thread([this] {
                    delete this;
                }).detach();
            } else {
                // TODO empty toBond queue
                closed_ = true;
                connection_->Shutdown(SHUT_RDWR);
            }

            return;
        }

        // send packet to connection
        int bytes_sent = 0;
        try {
            bytes_sent = connection_->Send(flowPacket->data(), flowPacket->header()->size(), 0);
            //DLOG(INFO) << "SENT " << bytes_sent << "bytes -> " << connection_->ToString();
        } catch (SocketErrorException e) {
            // socket closed waiting for close package
        } catch (Exception e) {
            LOG(ERROR) << e.ToString();
        }

        DLOG(INFO) << flowPacket->ToString() << " -> " << connection_->ToString();

        // delete package
        delete flowPacket;
    }

    void Flow::SendToBond() {
        // try to get next package
        packet::FlowPacket *flowPacket = toBondQueue_.Pop();

        // if next package is not available try again
        /*if( !flowPacket ) {
            usleep(40);
            return;
        }*/

        // close packet
        if( flowPacket->header()->size() == 0 ) {
            bond_->SendToTer(flowPacket);

            sendToBondLooper_.Stop();

            if (closed_) {
                std::thread([this] {
                    delete this;
                }).detach();
            } else {
                closed_ = true;
                connection_->Shutdown(SHUT_RDWR);
            }

            return;
        }

        DLOG(INFO) << "flow byteSize: " << byteSize() << ", flows byteSize: " << context_->flows()->getByteSize();

        // write packet
        if( byteSize() < 2000 || context_->flows()->getByteSize() < 74219 ) { // TODO dynamic TMC_THRESHOLDSMALLFLOW, TMC_THRESHOLDTERSAT, first packet?
            bond_->SendToTer(flowPacket);
        } else {
            bond_->SentToSat(flowPacket);
        }

        context_->flows()->subByteSize(flowPacket->header()->size());
    }

    Flow::~Flow() {
        LOG(INFO) << ToString() << ".~Flow()";

        // close and delete connection
        delete connection_;
    }

    uint64_t Flow::byteSize() {
        return toBondQueue_.byteSize();
    }

    std::string Flow::ToString() {
        return "Flow[source=" + source_.ToString() + ", destination=" + destination_.ToString() + "]";
    }

} // net