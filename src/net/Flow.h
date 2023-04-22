//
// Created by Matthias Hofstätter on 14.04.23.
//

#ifndef MULTIPATH_PROXY_FLOW_H
#define MULTIPATH_PROXY_FLOW_H

#include <queue>
#include "../packet/FlowPacket.h"
#include "base/TcpConnection.h"
#include "../collections/FlowPacketQueue.h"
#include "../handler/Handler.h"

namespace net {

    class Flow {
    public:
        static Flow *make(net::ipv4::TcpConnection *connection) {
            return new Flow(connection);
        }

        packet::FlowPacket *ReadFromFlow() {
            return rx_.pop();
        }

        void WriteToFlow(packet::FlowPacket *flowPacket) {
            tx_.push(flowPacket);
        }

        virtual ~Flow() {
            stop_.store(true, std::memory_order_release); // TODO implement Handler

            // stop threads
            recvHandler_->join(); // TODO implement Handler
            delete recvHandler_; // TODO implement Handler

            sendHandler_->join(); // TODO implement Handler
            delete sendHandler_; // TODO implement Handler

            // TODO remove and delete all packets in queue

            // close and delete connection
            delete connection_; // take care if multiple threads handle flow
        }

    protected:
        Flow(net::ipv4::TcpConnection *connection) : connection_(connection),
                                                     stop_(false), // TODO implement Handler
                                                     recvHandler_(new std::thread(&Flow::RecvFromConnection, this)), // TODO implement Handler
                                                     sendHandler_(new std::thread(&Flow::SendToConnection, this)), // TODO implement Handler
                                                     tx_(collections::FlowPacketQueue()),
                                                     rx_(collections::FlowPacketQueue()),
                                                     rxId_(1) { // currentId = 1, init packet = 0
            // add init FlowPacket into rx
            packet::header::FlowInitHeader flowInitHeader = packet::header::FlowInitHeader(connection_->GetPeerName(), connection_->GetSockName()); // TODO GetSock/PeerName local variable?
            packet::FlowPacket *flowPacket = packet::FlowPacket::make(flowInitHeader);

            rx_.push(flowPacket);
            tx_.push(flowPacket);

            LOG(INFO) << "created FlowInitPacket(0)[0]";
        }

    private:
        net::ipv4::TcpConnection *connection_;

        std::atomic_bool stop_; // TODO implement Handler
        std::thread *recvHandler_; // TODO implement Handler
        std::thread *sendHandler_; // TODO implement Handler

        collections::FlowPacketQueue tx_; // outgoing packages
        collections::FlowPacketQueue rx_; // incoming packages

        uint16_t rxId_;

        void RecvFromConnection() {
            LOG(INFO) << "Starting RecvFromConnection loop...";

            while(!stop_.load()) { // TODO implement Handler
                short events = connection_->Poll(POLLIN, 100);

                if (events & POLLIN) {
                    // create packet with data
                    packet::header::FlowHeader flowHeader = packet::header::FlowHeader(connection_->GetPeerName(),
                                                                                       connection_->GetSockName(),
                                                                                       rxId_); // TODO GetSock/PeerName local variable?
                    packet::FlowPacket *flowPacket = packet::FlowPacket::make(flowHeader, 1024); // TODO reserve()

                    // read available data from connection
                    ssize_t size = 0;
                    try {
                        size = connection_->Recv(flowPacket->data(), 1024, 0);
                    } catch (SocketClosedException e) {
                        // add close flow packet to queue
                        flowHeader = packet::header::FlowCloseHeader(connection_->GetPeerName(),connection_->GetSockName(),rxId_);
                        memcpy(flowPacket->header(), &flowHeader, sizeof(packet::header::FlowHeader));

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

        void SendToConnection() {
            LOG(INFO) << "Starting SendToConnection loop...";

            while(!stop_.load()) { // TODO implement Handler
                // sleep 10ms
                usleep(10000); // TODO implement Handler

                // if queue is empty do nothing
                if (tx_.empty()) {
                    continue;
                }

                // try to get next package
                packet::FlowPacket *flowPacket = tx_.pop();

                // if next package is not available do nothing
                if (!flowPacket) {
                    continue;
                }

                // init packet
                if (flowPacket->header()->ctrl() == packet::header::FLOW_CTRL::INIT) {
                    delete flowPacket;
                    continue;
                }

                // delete if close packet arrived
                if (flowPacket->header()->ctrl() == packet::header::FLOW_CTRL::CLOSE) {
                    stop_.store(true, std::memory_order_release); // TODO implement Handler
                    connection_->Close();
                    continue;
                }

                // send packet to connection
                connection_->Send(flowPacket->data(), flowPacket->header()->size(), 0);
            }

            LOG(INFO) << "Stopping SendToConnection loop...";
        }
    }; // Flow

} // net

#endif //MULTIPATH_PROXY_FLOW_H
