//
// Created by Matthias Hofstätter on 17.02.23.
//

#include <getopt.h>
#include "args.h"
#include "gui/gui.h"
#include <glog/logging.h>

namespace mpp::args {
    mode PROG = mode::LEFT;

    static const struct option long_options[] =
    {
            { "gui", no_argument,       0, 'g' },
            0
    };

    void init(int argc, char *argv[]) {
        LOG(INFO) << "Init args...";

        if (std::string(argv[0]) == "right") {
            PROG = mode::RIGHT;                                                                                     DLOG(INFO) << "Mode set to right";
        }

        while (1)
        {
            int index = -1;
            struct option * opt = 0;
            int result = getopt_long(argc, argv, "g", long_options, &index);
            if (result == -1) break; /* end of list */
            switch (result)
            {
                case 'g':
                    mpp::gui::ENABLED = true;                                                                             DLOG(INFO) << "Gui Flag set";
                    break;
                case 0: /* all parameter that do not */
                    /* appear in the optstring */
                    opt = (struct option *)&(long_options[index]);
                    printf("'%s' was specified.",
                           opt->name);
                    if (opt->has_arg == required_argument)
                        printf("Arg: <%s>", optarg);
                    printf("\n");
                    break;
                default: /* unknown */
                    break;
            }
        }
    }
}