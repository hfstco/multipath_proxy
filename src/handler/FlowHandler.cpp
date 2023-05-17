//
// Created by Matthias Hofstätter on 08.05.23.
//

#include <glog/logging.h>

#include "FlowHandler.h"

#include "../net/Flow.h"
#include "../net/Bond.h"
#include "../collections/FlowMap.h"

namespace handler {

    FlowHandler::FlowHandler(net::Flow *flow) : flow_(flow),
                                                                                                stopRecvFromConnection_(false),
                                                                                                recvFromConnectionHandler_(new std::thread(&FlowHandler::RecvFromConnection, this)),
                                                                                                stopSendToConnection_(false),
                                                                                                sendToConnectionHandler_(new std::thread(&FlowHandler::SendToConnection, this)),
                                                                                                stopSendToBond_(false),
                                                                                                sendToBondHandler_(new std::thread(&FlowHandler::SendToBond, this)),
                                                                                                closeFlowHandler_(new std::thread(&FlowHandler::CloseFlow, this))
                                                                                                { }

    FlowHandler *FlowHandler::make(net::Flow *flow) {
        return new FlowHandler(flow);
    }

    void FlowHandler::RecvFromConnection() {
        LOG(INFO) << "Starting RecvFromConnection loop...";

        while(!stopRecvFromConnection_.load()) { // TODO implement Handler
            try {
                flow_->RecvFromConnection();
            } catch (Exception e) {
                stopRecvFromConnection_.store(true);
            }
        }

        LOG(INFO) << "Stopping RecvFromConnection loop...";
    }

    void FlowHandler::SendToConnection() {
        LOG(INFO) << "Starting SendToConnection loop...";

        while(!stopSendToConnection_.load()) { // TODO implement Handler
            try {
                flow_->SendToConnection();
            } catch (Exception e) {
                stopSendToConnection_.store(true);
            }
        }

        LOG(INFO) << "Stopping SendToConnection loop...";
    }

    void FlowHandler::SendToBond() {
        LOG(INFO) << "Starting SendToBond loop...";

        while(!stopSendToBond_.load()) { // TODO implement Handler
            try {
                flow_->SendToBond();
            } catch (Exception e) {
                stopSendToBond_.store(true);
            }
        }

        LOG(INFO) << "Stopping SendToBond loop...";
    }

    FlowHandler::~FlowHandler() {
        LOG(INFO) << "~FlowHandler()";

        delete recvFromConnectionHandler_;
        delete sendToConnectionHandler_;
        delete sendToBondHandler_;

        delete flow_;
    }

    void FlowHandler::CloseFlow() {
        recvFromConnectionHandler_->join();
        sendToConnectionHandler_->join();
        sendToBondHandler_->join();

        delete this;
    }
}