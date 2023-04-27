//
// Created by Matthias Hofstätter on 14.04.23.
//

#include <thread>

#include "Flow.h"

#include "base/TcpConnection.h"
#include "../packet/FlowPacket.h"
#include "../packet/FlowHeader.h"
#include "Bond.h"

namespace net {
    Flow::Flow(net::ipv4::TcpConnection *pTcpConnection, net::Bond *pBond) : connection_(pTcpConnection), bond_(pBond),
    stop_(false), // TODO implement Handler
    recvFromConnectionHandler_(new std::thread(&Flow::RecvFromConnection, this)), // TODO implement Handler
    sendToConnectionHandler_(new std::thread(&Flow::SendToConnection, this)), // TODO implement Handler
    sendToBondHandler_(new std::thread(&Flow::SendToBond, this)),
    tx_(collections::FlowPacketQueue()),
    rx_(collections::FlowPacketQueue()),
    rxId_(1) { // currentId = 1, init packet = 0
        // add init FlowPacket into rx
        packet::FlowInitHeader flowInitHeader = packet::FlowInitHeader(connection_->GetPeerName(), connection_->GetSockName()); // TODO GetSock/PeerName local variable?
        packet::FlowPacket *pFlowPacket = packet::FlowPacket::make(flowInitHeader);

        rx_.push(pFlowPacket);

        pFlowPacket = packet::FlowPacket::make(flowInitHeader);
        tx_.push(pFlowPacket);

        LOG(INFO) << "created FlowInitPacket(0)[0]";
    }

    Flow *Flow::make(net::ipv4::TcpConnection *pTcpConnection, net::Bond *pBond) {
        return new Flow(pTcpConnection, pBond);
    }

    void Flow::WriteToFlow(packet::FlowPacket *pFlowPacket) {
        tx_.push(pFlowPacket);
    }

    void Flow::RecvFromConnection() {
        LOG(INFO) << "Starting RecvFromConnection loop...";

        while(!stop_.load()) { // TODO implement Handler
            short events = connection_->Poll(POLLIN, 100);

            if (events & POLLIN) {
                // create packet with data
                packet::FlowHeader flowHeader = packet::FlowHeader(connection_->GetPeerName(),
                                                                                   connection_->GetSockName(),
                                                                                   rxId_); // TODO GetSock/PeerName local variable?
                packet::FlowPacket *flowPacket = packet::FlowPacket::make(flowHeader, 1024); // TODO reserve()

                // read available data from connection
                ssize_t size = 0;
                try {
                    size = connection_->Recv(flowPacket->data(), 1024, 0);
                } catch (SocketClosedException e) {
                    // add close flow packet to queue
                    flowHeader = packet::FlowCloseHeader(connection_->GetPeerName(),connection_->GetSockName(),rxId_);
                    memcpy(flowPacket->header(), &flowHeader, sizeof(packet::FlowHeader));

                    // TODO shutdown flow because client has already closed socket?
                    stop_.store(true, std::memory_order_release); // TODO implement Handler
                } catch (Exception e) {
                    LOG(ERROR) << e.what();
                }

                // resize packet to fit data
                flowPacket->resize(size); // TODO avoid resize if possible

                // write package to rx queue
                rx_.push(flowPacket);

                LOG(INFO) << "created FlowPacket(" << rxId_ << ")[" << flowPacket->header()->size() << "]";

                // increment id
                rxId_ += 1;
            }
        }

        LOG(INFO) << "Stopping RecvFromConnection loop...";
    }

    void Flow::SendToConnection() {
        LOG(INFO) << "Starting SendToConnection loop...";

        while(!stop_.load()) { // TODO implement Handler
            // if queue is empty do nothing
            if (tx_.empty()) {
                // sleep 10ms
                usleep(10000); // TODO implement Handler
                continue;
            }

            // try to get next package
            packet::FlowPacket *flowPacket = tx_.pop();

            // if next package is not available do nothing
            if (!flowPacket) {
                // sleep 10ms
                usleep(10000); // TODO implement Handler
                continue;
            }

            // init packet
            if (flowPacket->header()->ctrl() == packet::FLOW_CTRL::INIT) {
                delete flowPacket;
                continue;
            }

            // delete if close packet arrived
            if (flowPacket->header()->ctrl() == packet::FLOW_CTRL::CLOSE) {
                stop_.store(true, std::memory_order_release); // TODO implement Handler
                connection_->Close();
                continue;
            }

            // send packet to connection
            connection_->Send(flowPacket->data(), flowPacket->header()->size(), 0);
        }

        LOG(INFO) << "Stopping SendToConnection loop...";
    }

    void Flow::SendToBond() {
        LOG(INFO) << "Starting SendToBond loop...";

        while(!stop_.load()) { // TODO implement Handler

            if(rx_.empty()) {
                // sleep 10ms
                usleep(10000); // TODO implement Handler
                continue;
            }

            // try to get next package
            packet::FlowPacket *pFlowPacket = rx_.pop();

            // if next package is not available do nothing
            if (!pFlowPacket) {
                // sleep 10ms
                usleep(10000); // TODO implement Handler
                continue;
            }

            bond_->WriteToBond(pFlowPacket);
        }

        LOG(INFO) << "Stopping SendToBond loop...";
    }

    Flow::~Flow() {
        stop_.store(true, std::memory_order_release); // TODO implement Handler

        // stop threads
        recvFromConnectionHandler_->join(); // TODO implement Handler
        delete recvFromConnectionHandler_; // TODO implement Handler

        sendToConnectionHandler_->join(); // TODO implement Handler
        delete sendToConnectionHandler_; // TODO implement Handler

        sendToBondHandler_->join(); // TODO implement Handler
        delete sendToBondHandler_; // TODO implement Handler

        // TODO remove and delete all packets in queue

        // close and delete connection
        delete connection_; // take care if multiple threads handle flow
    }
    
} // net