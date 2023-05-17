//
// Created by Matthias Hofstätter on 14.04.23.
//

#include <thread>
#include <assert.h>

#include "Flow.h"

#include "base/TcpConnection.h"
#include "../packet/FlowPacket.h"
#include "../packet/FlowHeader.h"
#include "Bond.h"
#include "../handler/FlowHandler.h"
#include "../collections/FlowMap.h"

namespace net {

    Flow::Flow(net::ipv4::SockAddr_In source, net::ipv4::SockAddr_In destination, net::ipv4::TcpConnection *pTcpConnection, collections::FlowMap *flows, net::Bond *bond) :   source_(source),
                                                                                                                                destination_(destination),
                                                                                                                                connection_(pTcpConnection),
                                                                                                                                bond_(bond),
                                                                                                                                flows_(flows),
                                                                                                                                toBondId_(0),
                                                                                                                                toConnectionQueue_(collections::FlowPacketQueue()),
                                                                                                                                toBondQueue_(collections::FlowPacketQueue()) { // currentId = 1, init packet = 0
        handler::FlowHandler::make(this);
        LOG(INFO) << "Flow(" << source_.ToString() << "|" << destination_.ToString() << ", " << pTcpConnection->ToString() << ")";
    }

    Flow *Flow::make(net::ipv4::SockAddr_In source, net::ipv4::SockAddr_In destination, net::ipv4::TcpConnection *pTcpConnection, collections::FlowMap *flows, net::Bond *bond) {
        return new Flow(source, destination, pTcpConnection, flows, bond);
    }

    void Flow::WriteToFlow(packet::FlowPacket *pFlowPacket) {
        toConnectionQueue_.Push(pFlowPacket);
    }

    void Flow::RecvFromConnection() {
        short events = connection_->Poll(POLLIN | POLLHUP | POLLNVAL, 100);

        if (events & POLLIN) {
            // create packet
            packet::FlowHeader flowHeader = packet::FlowHeader(source_, destination_, toBondId_++); // TODO GetSock/PeerName local variable?
            packet::FlowPacket *flowPacket = packet::FlowPacket::make(flowHeader, 1472 - sizeof(packet::FlowHeader)); // TODO reserve()

            // read data from connection
            ssize_t bytes_read = 0;
            try {
                LOG(INFO) << "CLIENT -> " << flowPacket->ToString();
                bytes_read = connection_->Recv(flowPacket->data(), 1472 - sizeof(packet::FlowHeader), 0);
            } catch (Exception e) {
                delete flowPacket;
                LOG(INFO) << e.what();
                throw e;
            }

            // resize packet to fit data
            flowPacket->Resize(bytes_read); // TODO avoid resize if possible

            assert(bytes_read == flowPacket->header()->size());
            assert(bytes_read == flowPacket->size() - sizeof(packet::FlowHeader));

            // write package to toBond_ queue
            toBondQueue_.Push(flowPacket);

            // stop handler
            if(bytes_read == 0) {
                throw ConnectionClosedException(flowPacket->ToString());
            }
        }

        //socket closed
        if( events & POLLNVAL ) {
            packet::FlowHeader flowHeader = packet::FlowHeader(source_, destination_, toBondId_++); // TODO GetSock/PeerName local variable?
            packet::FlowPacket *flowPacket = packet::FlowPacket::make(flowHeader, 0); // TODO reserve()

            toBondQueue_.Push(flowPacket);

            assert( flowPacket->header()->size() == 0 );
            assert( flowPacket->size() == sizeof(packet::FlowHeader) );

            throw ConnectionClosedException("POLLNVAL received.");
        }
    }

    void Flow::SendToConnection() {
        // try to get next package
        packet::FlowPacket *flowPacket = toConnectionQueue_.Pop();

        // if next package is not available
        if (!flowPacket) {
            usleep(10000);
            return;
        }

        // close packet
        if (flowPacket->header()->size() == 0) {
            delete flowPacket;
            //toBondQueue_.Clear();
            connection_->Close();
            throw GotClosePacketException("");
        }

        // send packet to connection
        try {
            LOG(INFO) << flowPacket->ToString() << " -> CLIENT";

            connection_->Send(flowPacket->data(), flowPacket->header()->size(), 0);
        } catch (Exception e) {
            LOG(INFO) << e.what();
        }

        // delete package
        delete flowPacket;
    }

    void Flow::SendToBond() {
        // try to get next package
        packet::FlowPacket *flowPacket = toBondQueue_.Pop();

        // if next package is not available
        if( !flowPacket ) {
            usleep(10000);
            return;
        }

        // close packet
        if( flowPacket->header()->size() == 0 ) {
            bond_->WriteToTer(flowPacket);
            throw GotClosePacketException(flowPacket->ToString());
        }

        // write packet
        if( toBondQueue_.byteSize() < 2000 || flows_->byteSize() < 74219 ) { // TODO dynamic TMC_THRESHOLDSMALLFLOW, TMC_THRESHOLDTERSAT
            bond_->WriteToTer(flowPacket);
        } else {
            bond_->WriteToSat(flowPacket);
        }
    }

    Flow::~Flow() {
        LOG(INFO) << "~Flow()";
        // close and delete connection
        delete connection_; // TODO take care if multiple threads handle flow

        flows_->Erase(source_, destination_);
    }

    uint64_t Flow::byteSize() {
        return toBondQueue_.byteSize();
    }

} // net