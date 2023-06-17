//
// Created by Matthias Hofstätter on 17.02.23.
//

#include <getopt.h>
#include "Args.h"
#include <glog/logging.h>

namespace args {

    // mode MODE = mode::LOCAL;
    // std::string TER;
    // std::string SAT;

    void printHelp() {
        exit(1);
    }

    void init(int argc, char *argv[]) {
        LOG(INFO) << "Init args...";

        if (std::string(argv[0]) == "remote") {
            MODE = mode::REMOTE;                                                                                     DLOG(INFO) << "Mode set to right";
        }

        const char* const short_opts = "rlt:s:gh";
        const struct option long_opts[] =
        {
                {"remote", no_argument,       0, 'r'},
                {"local", no_argument,       0, 'l'},
                {"ter", required_argument,       0, 't'},
                {"sat", required_argument,       0, 's'},
                //{"gui", no_argument,       0, 'g'},
                {"metrics", no_argument, 0, 'm'},
                {"help", no_argument, nullptr, 'h'},
                0
        };

        while (true)
        {
            int index = -1;
            struct option *opt = 0;
            int result = getopt_long(argc, argv, short_opts, long_opts, &index);
            if (result == -1) break; /* end of list */
            switch (result)
            {
                case 'r':
                    MODE = mode::REMOTE;
                    break;

                case 'l':
                    MODE = mode::LOCAL;
                    break;

                case 't':
                    opt = (struct option *)&(long_opts[index]);
                    TER = optarg;
                    LOG(INFO) << "TER=" + TER;
                    break;

                case 's':
                    opt = (struct option *)&(long_opts[index]);
                    SAT = optarg;
                    LOG(INFO) << "SAT=" + SAT;
                    break;

                case 'm':
                    opt = (struct option *)&(long_opts[index]);
                    METRICS_ENABLED = true;
                    LOG(INFO) << "Metrics enabled.";
                    break;

                case 0:
                    opt = (struct option *)&(long_opts[index]);
                    printf("'%s' was specified.",
                           opt->name);
                    if (opt->has_arg == required_argument)
                        printf("Arg: <%s>", optarg);
                    printf("\n");
                    break;

                default:
                    break;
            }
        }
    }

} // mpp::args