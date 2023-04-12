//
// Created by Matthias Hofstätter on 30.03.23.
//

#include "gtest/gtest.h"
#include "../src/exception/Exception.h"

TEST(exception_Exception, NetworkError_what_str) {
    std::string str1 = "test1";
    Exception e1 = Exception(str1);

    EXPECT_EQ(str1, e1.what());
}

TEST(exception_Exception, NetworkError_what_char_array) {
    char* cs1 = "test2";
    NetworkError ne1 = NetworkError(cs1);

    EXPECT_EQ(0, strcmp(cs1, ne1.what()));
}
