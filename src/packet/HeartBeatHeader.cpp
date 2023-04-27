//
// Created by Matthias Hofstätter on 25.04.23.
//

#include "HeartBeatHeader.h"

namespace packet {

    HeartBeatHeader::HeartBeatHeader() : Header(TYPE::HEARTBEAT) {}

    std::string HeartBeatHeader::ToString() {
        return "HBH[]";
    }

}