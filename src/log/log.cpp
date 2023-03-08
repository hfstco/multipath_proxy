//
// Created by Matthias Hofstätter on 17.02.23.
//

#include "log.h"
#include "../gui/gui.h"
#include "glog/logging.h"

namespace mpp::log {
    std::string FILE = "mpp.log";

    void init() {
        google::SetLogDestination(0,"mpp.log");
        google::InitGoogleLogging("log_test");
        if(!mpp::gui::ENABLED) {
            FLAGS_alsologtostderr = 1;
        }
    }
}