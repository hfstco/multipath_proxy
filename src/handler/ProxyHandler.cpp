//
// Created by Matthias Hofstätter on 08.05.23.
//

#include <glog/logging.h>

#include "ProxyHandler.h"
#include "../net/Proxy.h"
#include "../net/Bond.h"
#include "../collections/FlowMap.h"
#include "../exception/Exception.h"

namespace handler {
    ProxyHandler *ProxyHandler::make(net::Proxy *proxy, collections::FlowMap *flows, net::Bond *bond) {
        return new ProxyHandler(proxy, flows, bond);
    }

    ProxyHandler::ProxyHandler(net::Proxy *proxy, collections::FlowMap *flows, net::Bond *bond) :   proxy_(proxy),
                                                                                                    flows_(flows),
                                                                                                    bond_(bond),
                                                                                                    stopAcceptHandler_(false),
                                                                                                    acceptHandler_(new std::thread(&ProxyHandler::Accept, this))
    {

    }

    void ProxyHandler::Accept() {
        LOG(INFO) << "Starting Accept loop...";

        while (!stopAcceptHandler_.load()) {
            try {
                proxy_->Accept(flows_, bond_);
            } catch (ListenerClosedException e) {
                stopAcceptHandler_.store(true);
                delete this;
            }
        }

        LOG(INFO) << "Stopping Accept loop...";
    }

    ProxyHandler::~ProxyHandler() {
        join();

        delete proxy_;
    }

    void ProxyHandler::join() {
        acceptHandler_->join();
    }
} // handler