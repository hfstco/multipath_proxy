//
// Created by Matthias Hofstätter on 04.04.23.
//

#include <gtest/gtest.h>

#include "../src/utils/Utils.h"

TEST(utils_Util, string_to_in_addr_and_back) {
    std::string ip1 = "127.0.0.1";

    in_addr inAddr1 = utils::StringToInAddr(ip1);

    std::string ip2 = utils::InAddrToString(inAddr1);

    EXPECT_EQ(ip1, ip2);
}

TEST(utils_Util, string_to_in6_addr_and_back) {
    std::string ip1 = "::1";

    in6_addr in6Addr1 = utils::StringToIn6Addr(ip1);

    std::string ip2 = utils::In6AddrToString(in6Addr1);

    EXPECT_EQ(ip1, ip2);
}