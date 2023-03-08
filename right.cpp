//
// Created by Matthias Hofstätter on 15.02.23.
//

#include "glog/logging.h"
#include "right.h"
#include "src/main.h"
#include "src/net/Connection.h"

namespace mpp {

}

int main(int argc, char *argv[]) {
    mpp::main::init(argc, argv);
    LOG(INFO) << "Right mode initialized.";

    mpp::connection con1 = mpp::connection("192.168.2.1", "192.168.2.2", 50000);
    con1.connect();

    std::vector<char> test1;

    mpp::connection con2 = mpp::connection("192.168.3.1", "192.168.3.2", 50000);
    con2.connect();

    std::vector<char> test2;

    return 0;
}