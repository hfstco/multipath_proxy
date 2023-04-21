//
// Created by Matthias Hofstätter on 04.04.23.
//

#include <gtest/gtest.h>

#include "../src/utils/Utils.h"

TEST(utils_Utils, split_ip_and_port) {
    std::string str1("127.0.0.1:4321");
    auto ipandport = utils::splitIPandPort(str1);

    EXPECT_EQ(ipandport.ip, "127.0.0.1");
    EXPECT_EQ(ipandport.port, 4321);
}

TEST(utils_Utils, connection_string) {
    auto connectionString = utils::connectionString("127.0.0.1", 2345, "10.0.0.1", 8543);

    EXPECT_EQ(connectionString, "127.0.0.1:2345|10.0.0.1:8543");
}