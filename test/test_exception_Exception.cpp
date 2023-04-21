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
    std::string str1 = "test1";
    char *cs1 = str1.data();
    NetworkException ne1 = NetworkException(cs1);

    EXPECT_EQ(0, strcmp(cs1, ne1.what()));
}
