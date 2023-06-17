//
// Created by Matthias Hofstätter on 17.02.23.
//

#ifndef MULTIPATH_PROXY_ARGS_H
#define MULTIPATH_PROXY_ARGS_H

#include <string>


namespace args {

    enum class mode : bool {
        LOCAL = false,
        REMOTE = true
    };

    inline mode MODE;
    inline std::string TER;
    inline std::string SAT;

    inline bool METRICS_ENABLED = false;

    void init(int argc, char *argv[]);

} // mpp::args


#endif //MULTIPATH_PROXY_ARGS_H
