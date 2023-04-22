//
// Created by Matthias Hofstätter on 18.04.23.
//

#ifndef MULTIPATH_PROXY_BOND_H
#define MULTIPATH_PROXY_BOND_H

#include "base/TcpConnection.h"
#include "../collections/FlowMap.h"

namespace net {

    class Bond {
    public:
        Bond(net::ipv4::TcpConnection *ter, net::ipv4::TcpConnection *sat, collections::FlowMap *flows) : ter_(ter),
                                                                                                          sat_(sat),
                                                                                                          flows_(flows),
                                                                                                          stop_(false),
                                                                                                          readTerHandler_(),
                                                                                                          readSatHandler_(),
                                                                                                          writeBondHandler_() {}

        virtual ~Bond() {
        }

    private:
        net::ipv4::TcpConnection *ter_, *sat_;
        collections::FlowMap *flows_;

        std::atomic_bool stop_; // TODO implement Handler
        std::thread *readTerHandler_; // TODO implement Handler
        std::thread *readSatHandler_; // TODO implement Handler
        std::thread *writeBondHandler_; // TODO implement Handler
        // std::thread *bondManager_; // TODO implement Handler

        void WriteToBond(packet::FlowPacket *flowPacket) {
            while (!stop_.load()) {
                // loop flows for new packets

            }
            // send control packages over ter_
            if (flowPacket->header()->ctrl() == packet::header::FLOW_CTRL::CLOSE || flowPacket->header()->ctrl() == packet::header::FLOW_CTRL::INIT) {
                // TODO write to TER
            }

            // TODO which connection to use
            // TODO write to TER || SAT
        }

        void ReadFromConnection(net::ipv4::TcpConnection *connection) {
            while (!stop_.load()) {
                int events = connection->Poll(POLLIN, 100);

                if (events & POLLIN) {
                    // create buffer
                    packet::Buffer *buffer = packet::Buffer::make(1024);

                    // read header
                    try {
                        connection->Recv(buffer->data(), sizeof(packet::header::Header), 0);
                    } catch (SocketClosedException e) {
                        // TODO
                    } catch (SocketErrorException e) {
                        // TODO
                    }

                    packet::Packet *packet = (packet::Packet *)buffer;

                    // check if HeartBeatHeader
                    if (packet->header()->type() == packet::header::TYPE::HEARTBEAT) {
                        delete buffer;
                        continue;
                    }

                    assert(packet->header()->type() == packet::header::TYPE::FLOW);

                    // read full header
                    try {
                        connection->Recv((unsigned char*)packet->header() + sizeof(packet::header::Header), sizeof(packet::header::FlowHeader) - sizeof(packet::header::Header), 0);
                    } catch (SocketClosedException e) {
                        // TODO
                    } catch (SocketErrorException e) {
                        // TODO
                    }

                    packet::FlowPacket *flowPacket = (packet::FlowPacket *)packet;

                    // create flow if init packet
                    if (flowPacket->header()->ctrl() == packet::header::FLOW_CTRL::INIT) {
                        // check if flow already created
                        // if ( flowVector.find("127.0.0.1:2345|192.168.18.1:3456") )
                        //      return nullptr;

                        // create flow
                        net::ipv4::SockAddr_In flowSockAddrIn = net::ipv4::SockAddr_In(
                                flowPacket->header()->destinationIp(), flowPacket->header()->destinationPort());
                        net::ipv4::TcpConnection *flowConnection = net::ipv4::TcpConnection::make(flowSockAddrIn);
                        net::Flow *flow = net::Flow::make(flowConnection);

                        // TODO add Flow to FlowVector

                        continue;
                    }

                    // read packet data from socket
                    try {
                        connection->Recv(flowPacket->data(), flowPacket->header()->size(), 0);
                    } catch (SocketClosedException e) {
                        // TODO
                    } catch (SocketErrorException e) {
                        // TODO
                    }
                    flowPacket->resize(flowPacket->header()->size()); // TODO avoid resize if possible

                    // TODO Flow regular packet before Flow init packet

                    // TODO find Flow in FlowVector and push FlowPacket
                }
            }
        }
    };

} // net

#endif //MULTIPATH_PROXY_BOND_H
