//
// Created by Matthias Hofstätter on 24.02.23.
//


#include <gtest/gtest.h>
#include <glog/logging.h>

GTEST_API_ int main(int argc, char **argv) {
    google::InitGoogleLogging("log_test");
    FLAGS_logtostderr = 1;
    FLAGS_v = 10;
    testing::InitGoogleTest(&argc, argv);
    if (VLOG_IS_ON(1)) {
        return RUN_ALL_TESTS();
    }
    return RUN_ALL_TESTS();
}