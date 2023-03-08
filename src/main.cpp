#include "gui/gui.h"
#include <thread>
#include "glog/logging.h"
#include "log/log.h"
#include "args.h"

namespace mpp {
    namespace main {
        void init(int argc, char *argv[]) {
            mpp::log::init();
            mpp::args::init(argc, argv);

            LOG(INFO) << "Starting multipath_proxy...";

            if(mpp::gui::ENABLED) {
                mpp::gui::init();
                mpp::gui::start();
            }

            if(mpp::gui::ENABLED) {
                mpp::gui::stop();
            }
        }
    }
}
