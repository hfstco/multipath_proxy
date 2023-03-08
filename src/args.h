//
// Created by Matthias Hofstätter on 17.02.23.
//

#ifndef MULTIPATH_PROXY_ARGS_H
#define MULTIPATH_PROXY_ARGS_H


namespace mpp::args {
    enum class mode : bool {
        LEFT = false,
        RIGHT = true
    };

    extern mode PROG;

    void init(int argc, char *argv[]);
}


#endif //MULTIPATH_PROXY_ARGS_H
