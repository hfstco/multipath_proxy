//
// Created by Matthias Hofstätter on 10.08.23.
//

#include <gtest/gtest.h>
#include <glog/logging.h>


TEST(skip_connection, skip) {
    // input params
    bool issat = false;

    uint64_t backlog = 2000;
    uint64_t totalBacklog = 74000;
    bool usesat = false;

    bool ter = true;
    bool sat = true;

    //if (issat) {
        if (!usesat) {
            if (totalBacklog < 74219 || backlog <= 2000) {
                sat = false;
            }
        }

        //usesat = true;
    //} else {
        if (usesat || (backlog > 2000 && totalBacklog >= 74219)) {
            ter = false;
        }
    //}

    CHECK(sat != ter); // only one of the connection should be chosen
    CHECK(sat == usesat); // when sat is used usesat should set to true
}
