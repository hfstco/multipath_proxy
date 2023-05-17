//
// Created by Matthias Hofstätter on 09.05.23.
//

#include <glog/logging.h>

#include "BondHandler.h"

#include "../net/Bond.h"

namespace handler {
    BondHandler::BondHandler(net::Bond *bond) :
            bond_(bond),
            stopReadTerHandler_(false),
            stopReadSatHandler_(false),
            stopWriteHeartBeatPacket_(false),
            stopCheckBondBuffers_(false),
            readTerHandler_(new std::thread(&BondHandler::ReadFromTerConnection, this)),
            readSatHandler_(new std::thread(&BondHandler::ReadFromSatConnection, this)),
            writeHeartBeatHandler_(new std::thread(&BondHandler::WriteHeartBeatPacket, this)),
            checkBondBuffersHandler_(new std::thread(&BondHandler::CheckBondBuffers, this)) {}

    BondHandler *BondHandler::make(net::Bond *bond) {
        return new BondHandler(bond);
    }

    BondHandler::~BondHandler() {

    }

    void BondHandler::ReadFromTerConnection() {
        LOG(INFO) << "Starting ReadFromTerConnection loop...";

        while(!stopReadTerHandler_.load()) { // TODO implement Handler
            try {
                bond_->ReadFromTerConnection();
            } catch (Exception e) {
                LOG(INFO) << e.what();
            }
        }

        LOG(INFO) << "Stopping ReadFromTerConnection loop...";
    }

    void BondHandler::ReadFromSatConnection() {
        LOG(INFO) << "Starting ReadFromSatConnection loop...";

        while(!stopReadSatHandler_.load()) { // TODO implement Handler
            try {
                bond_->ReadFromSatConnection();
            } catch (Exception e) {
                LOG(INFO) << e.what();
            }
        }

        LOG(INFO) << "Stopping ReadFromSatConnection loop...";
    }

    void BondHandler::WriteHeartBeatPacket() {
        LOG(INFO) << "Starting WriteHeartBeatPacket loop...";

        while(!stopWriteHeartBeatPacket_.load()) { // TODO implement Handler
            try {
                bond_->WriteHeartBeatPacket();
                usleep(1000000);
            } catch (Exception e) {
                LOG(INFO) << e.what();
            }
        }

        LOG(INFO) << "Stopping WriteHeartBeatPacket loop...";
    }

    void BondHandler::CheckBondBuffers() {
        LOG(INFO) << "Starting CheckBondBuffers loop...";

        while(!stopCheckBondBuffers_.load()) { // TODO implement Handler
            try {
                bond_->CheckBondBuffers();
                usleep(1000000);
            } catch (Exception e) {
                LOG(INFO) << e.what();
            }
        }

        LOG(INFO) << "Stopping CheckBondBuffers loop...";
    }


} // handler