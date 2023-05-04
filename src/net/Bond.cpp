//
// Created by Matthias Hofstätter on 18.04.23.
//

#include <cassert>

#include "Bond.h"

#include "../packet/FlowPacket.h"
#include "../packet/FlowHeader.h"
#include "base/TcpConnection.h"
#include "Flow.h"

namespace net {
    Bond::Bond(net::ipv4::TcpConnection *ter, net::ipv4::TcpConnection *sat, collections::FlowMap *flows) : ter_(ter),
    sat_(sat),
    flows_(flows),
    stop_(false),
    readTerHandler_(new std::thread(&Bond::ReadFromConnection, this, ter_)),
    readSatHandler_(new std::thread(&Bond::ReadFromConnection, this, sat_)) {}

    Bond *Bond::make(net::ipv4::TcpConnection *ter, net::ipv4::TcpConnection *sat, collections::FlowMap *flows) {
        return new Bond(ter, sat, flows);
    }

    void Bond::WriteToBond(packet::FlowPacket *pFlowPacket) {
        packet::Buffer *pBuffer = pFlowPacket;

        // send control packages over ter_
        if (pFlowPacket->header()->ctrl() == packet::FLOW_CTRL::CLOSE || pFlowPacket->header()->ctrl() == packet::FLOW_CTRL::INIT) {
            try {
                ter_->Send(pBuffer->data(), pBuffer->size(), 0);
            } catch (Exception e) {
                // TODO
                LOG(ERROR) << e.what();
            }
        }

        assert(pFlowPacket->header()->ctrl() != packet::FLOW_CTRL::REGULAR);

        // TODO which connection to use
        // TODO write to TER || SAT
        try {
            sat_->Send(pBuffer->data(), pBuffer->size(), 0); // TODO testing
        } catch (Exception e) {
            // TODO
            LOG(ERROR) << e.what();
        }
    }

    void Bond::ReadFromConnection(net::ipv4::TcpConnection *connection) {
        while (!stop_.load()) {
            int events = connection->Poll(POLLIN, 100);

            if (events & POLLIN) {
                // create buffer
                packet::Buffer *buffer = packet::Buffer::make(1024);

                // read header
                try {
                    connection->Recv(buffer->data(), sizeof(packet::Header), 0);
                } catch (SocketClosedException e) {
                    // TODO
                    stop_.store(true);
                    continue;
                } catch (SocketErrorException e) {
                    // TODO
                    stop_.store(true);
                    continue;
                }

                packet::Packet *packet = (packet::Packet *)buffer;

                // check if HeartBeatHeader
                if (packet->header()->type() == packet::TYPE::HEARTBEAT) {
                    delete buffer;
                    continue;
                }

                assert(packet->header()->type() == packet::TYPE::FLOW);

                // read full header
                try {
                    connection->Recv((unsigned char*)packet->header() + sizeof(packet::Header), sizeof(packet::FlowHeader) - sizeof(packet::Header), 0);
                } catch (SocketClosedException e) {
                    // TODO
                    stop_.store(true);
                    continue;
                } catch (SocketErrorException e) {
                    // TODO
                    stop_.store(true);
                    continue;
                }

                packet::FlowPacket *flowPacket = (packet::FlowPacket *)packet;

                // create flow if init packet
                if (flowPacket->header()->ctrl() == packet::FLOW_CTRL::INIT) {
                    // check if flow already created
                    // if ( flowVector.find("127.0.0.1:2345|192.168.18.1:3456") )
                    //      return nullptr;

                    // create flow
                    net::ipv4::SockAddr_In flowSockAddrIn = net::ipv4::SockAddr_In(flowPacket->header()->destinationIp(), flowPacket->header()->destinationPort());
                    net::ipv4::TcpConnection *flowConnection = net::ipv4::TcpConnection::make(flowSockAddrIn);
                    net::Flow *flow = net::Flow::make(flowConnection, this);

                    // TODO add Flow to FlowVector

                    continue;
                }

                // read packet data from socket
                try {
                    connection->Recv(flowPacket->data(), flowPacket->header()->size(), 0);
                } catch (SocketClosedException e) {
                    // TODO
                    stop_.store(true);
                    continue;
                } catch (SocketErrorException e) {
                    // TODO
                    stop_.store(true);
                    continue;
                }
                flowPacket->resize(flowPacket->header()->size()); // TODO avoid resize if possible

                // TODO Flow regular packet before Flow init packet

                // TODO find Flow in FlowVector and push FlowPacket
            }
        }
    }

    Bond::~Bond() {
    }

} // net