//
// Created by Matthias Hofstätter on 18.04.23.
//

#include <cassert>

#include "Bond.h"

#include "../packet/FlowPacket.h"
#include "../packet/FlowHeader.h"
#include "base/TcpConnection.h"
#include "Flow.h"
#include "../collections/FlowMap.h"
#include "../handler/FlowHandler.h"
#include "../packet/HeartBeatPacket.h"
#include "../handler/BondHandler.h"

namespace net {
    Bond::Bond(net::ipv4::TcpConnection *terConnection, net::ipv4::TcpConnection *satConnection, collections::FlowMap *flows) : flows_(flows), terConnection_(terConnection), satConnection_(satConnection) {
    }

    Bond *Bond::make(net::ipv4::TcpConnection *terConnection, net::ipv4::TcpConnection *satConnection, collections::FlowMap *flows) {
        net::Bond *bond = new Bond(terConnection, satConnection, flows);
        handler::BondHandler::make(bond);
        return bond;
    }

    void Bond::ReadFromConnection(net::ipv4::TcpConnection *connection) {
        if( connection->Poll(POLLIN, 100) == POLLIN ) {
            // create buffer
            packet::Buffer *buffer = packet::Buffer::make(1472);

            // read Header
            int bytes_read = 0;
            while( bytes_read < sizeof(packet::Header) ) {
                bytes_read += connection->Recv(buffer->data() + bytes_read, sizeof(packet::Header) - bytes_read, 0);
            }

            assert( bytes_read == sizeof(packet::Header) );

            packet::Packet *packet = (packet::Packet *)buffer;

            // skip HeartBeatHeader
            if( packet->header()->type() == packet::TYPE::HEARTBEAT ) {
                delete buffer;
                return ReadFromConnection(connection); // skip
            }

            assert( packet->header()->type() == packet::TYPE::FLOW );

            // read FlowHeader
            while( bytes_read < sizeof(packet::FlowHeader) ) {
                bytes_read += connection->Recv((unsigned char*)packet->header() + bytes_read, sizeof(packet::FlowHeader) - bytes_read, 0);
            }

            assert( bytes_read == sizeof(packet::FlowHeader) );

            packet::FlowPacket *flowPacket = (packet::FlowPacket *)packet;

            flowPacket->Resize(flowPacket->header()->size()); // TODO avoid resize if possible

            // read packet data from socket
            while(bytes_read < sizeof(packet::FlowHeader) + flowPacket->header()->size() ) {
                bytes_read += connection->Recv(flowPacket->data(), sizeof(packet::FlowHeader) + flowPacket->header()->size() - bytes_read, 0);
            }

            assert(bytes_read == sizeof(packet::FlowHeader) + flowPacket->header()->size());

            if(flowPacket->header()->size() == 0)
                LOG(INFO) << "ReadFromConnection(" << connection->ToString() << ") -> " << flowPacket->ToString();

            createFlowMutex_.lock();
            // check if flow already created
            if ( !flows_->Contains(flowPacket->header()->source(), flowPacket->header()->destination()) ) {
                // create flow
                try {
                    net::ipv4::TcpConnection *flowConnection = net::ipv4::TcpConnection::make(flowPacket->header()->destination());
                    net::Flow *flow = net::Flow::make(flowPacket->header()->source(), flowPacket->header()->destination(), flowConnection, flows_, this);

                    flows_->Insert(flowPacket->header()->source(), flowPacket->header()->destination(), flow);
                } catch (Exception e) {
                    LOG(INFO) << e.what();
                }
            }
            createFlowMutex_.unlock();

            // find flow
            net::Flow *pFlow = flows_->Find(flowPacket->header()->source(), flowPacket->header()->destination());
            pFlow->WriteToFlow(flowPacket);
        }
    }

    Bond::~Bond() {
    }

    std::string Bond::ToString() {
        return "Bond[ter=" + terSockAddr_.ToString() + ", sat=" + satSockAddr_.ToString() + ", " + flows_->ToString() + "]";
    }

    void Bond::ReadFromTerConnection() {
        ReadFromConnection(terConnection_);
    }

    void Bond::ReadFromSatConnection() {
        ReadFromConnection(satConnection_);
    }

    void Bond::WriteHeartBeatPacket() {
        writeSatMutex_.lock();
        WriteToConnection(satConnection_, packet::HeartBeatPacket::make());
        writeSatMutex_.unlock();
    }

    void Bond::WriteToConnection(net::ipv4::TcpConnection *connection, packet::Packet *packet) {
        packet::Buffer *buffer = packet;

        try {
            int bytes_written = 0;
            while (bytes_written < buffer->size()) {
                bytes_written += connection->Send(buffer->data() + bytes_written, buffer->size() - bytes_written, 0);
            }

            assert(bytes_written == buffer->size());
        } catch (Exception e) {
            LOG(INFO) << e.what();
        }
    }

    void Bond::CheckBondBuffers() {

    }

    void Bond::WriteToTer(packet::FlowPacket *flowPacket) {
        LOG(INFO) << flowPacket->ToString() << " -> TER";

        assert(flowPacket->header()->ctrl() == packet::FLOW_CTRL::REGULAR);
        assert(flowPacket->size() == flowPacket->header()->size() + sizeof(packet::FlowHeader));

        writeTerMutex_.lock();
        WriteToConnection(terConnection_, flowPacket);
        writeTerMutex_.unlock();
    }

    void Bond::WriteToSat(packet::FlowPacket *flowPacket) {
        LOG(INFO) << flowPacket->ToString() << " -> SAT";

        assert(flowPacket->header()->ctrl() == packet::FLOW_CTRL::REGULAR);
        assert(flowPacket->size() == flowPacket->header()->size() + sizeof(packet::FlowHeader));

        writeSatMutex_.lock();
        WriteToConnection(satConnection_, flowPacket);
        writeSatMutex_.unlock();
    }

} // net