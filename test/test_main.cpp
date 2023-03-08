//
// Created by Matthias Hofstätter on 24.02.23.
//


#include <gtest/gtest.h>
#include <glog/logging.h>

int main(int argc, char **argv) {
    google::InitGoogleLogging("log_test");
    FLAGS_logtostderr = 1;
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}