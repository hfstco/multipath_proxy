//
// Created by Matthias Hofstätter on 08.05.23.
//

#ifndef MULTIPATH_PROXY_FLOWHANDLER_H
#define MULTIPATH_PROXY_FLOWHANDLER_H

#include <atomic>
#include <thread>

namespace net {
    class Flow;
    class Bond;
}

namespace collections {
    class FlowMap;
}

namespace handler {
    class FlowHandler {
    public:
        static FlowHandler *make(net::Flow *flow);

        virtual ~FlowHandler();

    protected:
        FlowHandler(net::Flow *flow);

    private:
        net::Flow *flow_;

        std::atomic_bool stopRecvFromConnection_; // TODO implement Handler
        std::thread *recvFromConnectionHandler_; // TODO implement Handler

        std::atomic_bool stopSendToConnection_; // TODO implement Handler
        std::thread *sendToConnectionHandler_; // TODO implement Handler

        std::atomic_bool stopSendToBond_; // TODO implement Handler
        std::thread *sendToBondHandler_; // TODO implement Handler

        std::thread *closeFlowHandler_;

        void RecvFromConnection();
        void SendToConnection();

        void SendToBond();

        void CloseFlow();
    };
}


#endif //MULTIPATH_PROXY_FLOWHANDLER_H
