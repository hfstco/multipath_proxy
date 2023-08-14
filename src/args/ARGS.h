//
// Created by Matthias Hofstätter on 17.02.23.
//

#ifndef MULTIPATH_PROXY_ARGS_H
#define MULTIPATH_PROXY_ARGS_H

#include <string>
#include <getopt.h>
#include <glog/logging.h>

namespace args {
    enum class mode : bool {
        LOCAL = false,
        REMOTE = true
    };

    inline mode MODE = mode::LOCAL;

    inline bool TER_ENABLED = false;
    inline std::string TER_STRING;

    inline bool SAT_ENABLED = false;
    inline std::string SAT_STRING;

    inline std::string PROXY_STRING;

    inline bool HEARTBEAT_ENABLED = false;

    inline bool METRICS_ENABLED = false;

    static void init(int argc, char *argv[]) {
        LOG(INFO) << "Init args...";

        const char* const short_opts = "rlt:s:p:bghm";
        const struct option long_opts[] =
                {
                        {"remote", no_argument,       nullptr, 'r'},
                        {"local", no_argument,       nullptr, 'l'},
                        {"ter", required_argument,       nullptr, 't'},
                        {"sat", required_argument,       nullptr, 's'},
                        {"proxy", optional_argument, nullptr, 'p'},
                        {"heartbeat", no_argument,      nullptr, 'b'},
                        //{"gui", no_argument,       0, 'g'},
                        {"metrics", no_argument, nullptr, 'm'},
                        {"help", no_argument, nullptr, 'h'},
                        nullptr
                };

        while (true)
        {
            int index = -1;
            struct option *opt = nullptr;
            int result = getopt_long(argc, argv, short_opts, long_opts, &index);
            if (result == -1) break; /* end of list */
            switch (result)
            {
                case 'r':
                    args::MODE = args::mode::REMOTE;
                    break;

                case 'l':
                    args::MODE = args::mode::LOCAL;
                    break;

                case 't':
                    opt = (struct option *)&(long_opts[index]);
                    args::TER_ENABLED = true;
                    args::TER_STRING = optarg;
                    LOG(INFO) << "TER=" + args::TER_STRING;
                    break;

                case 's':
                    opt = (struct option *)&(long_opts[index]);
                    args::SAT_ENABLED = true;
                    args::SAT_STRING = optarg;
                    LOG(INFO) << "SAT=" + args::SAT_STRING;
                    break;

                case 'p':
                    opt = (struct option *)&(long_opts[index]);
                    args::PROXY_STRING = optarg;
                    LOG(INFO) << "PROXY=" + args::PROXY_STRING;
                    break;

                case 'b':
                    args::HEARTBEAT_ENABLED = true;
                    LOG(INFO) << "Heartbeat enabled.";
                    break;

                case 'm':
                    args::METRICS_ENABLED = true;
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

        if (args::MODE == args::mode::LOCAL && args::PROXY_STRING.empty()) {
            LOG(ERROR) << "PROXY (-p) not defined.";
            exit(-1);
        }
    }

} // args

#endif //MULTIPATH_PROXY_ARGS_H
