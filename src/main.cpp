//
// Created by Matthias Hofstätter on 04.04.23.
//

#include "base/Local.h"
#include "base/Remote.h"
#include "args/Args.h"
#include "log/Log.h"

int main(int argc, char *argv[]) {
    // init logging
    log::init();

    // init arguments
    args::init(argc, argv);

    // init mpp
    if(args::MODE == args::mode::REMOTE) {

    } else {

    }

    return 0;
}