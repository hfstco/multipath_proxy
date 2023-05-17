//
// Created by Matthias Hofstätter on 09.05.23.
//

#ifndef MULTIPATH_PROXY_BONDHANDLER_H
#define MULTIPATH_PROXY_BONDHANDLER_H

#include <atomic>
#include <thread>

namespace net {
    namespace ipv4 {
        class TcpConnection;
    }

    class Bond;
}

namespace handler {

    class BondHandler {
    public:
        static BondHandler *make(net::Bond *bond);

        virtual ~BondHandler();

    private:
        net::Bond *bond_;

        std::atomic_bool stopReadTerHandler_; // TODO implement Handler
        std::atomic_bool stopReadSatHandler_; // TODO implement Handler
        std::atomic_bool stopWriteHeartBeatPacket_; // TODO implement Handler
        std::atomic_bool stopCheckBondBuffers_;
        std::thread *readTerHandler_; // TODO implement Handler
        std::thread *readSatHandler_; // TODO implement Handler
        std::thread *writeHeartBeatHandler_; // TODO implement Handler
        std::thread *checkBondBuffersHandler_;

        BondHandler(net::Bond *bond);

        void ReadFromTerConnection();
        void ReadFromSatConnection();
        //void WriteToBond();
        void WriteHeartBeatPacket();
        void CheckBondBuffers();
    };

} // handler

#endif //MULTIPATH_PROXY_BONDHANDLER_H
