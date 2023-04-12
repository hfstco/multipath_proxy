//
// Created by Matthias Hofstätter on 17.02.23.
//

#include "Log.h"
#include "../gui/Gui.h"
#include "glog/logging.h"


namespace log {
    std::string FILE = "mpp.log";

    void init() {
        google::SetLogDestination(0,"mpp.log");
        google::InitGoogleLogging("log_test");
        if(!gui::ENABLED) {
            FLAGS_alsologtostderr = 1;
        }
    }
}