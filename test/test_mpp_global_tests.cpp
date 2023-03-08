//
// Created by Matthias Hofstätter on 24.02.23.
//

#include <gtest/gtest.h>

TEST(test_mpp_global_tests, std__string_to_std__vector_char) {
    std::string str1("helloworld!");
    std::vector<char> vec1(str1.begin(), str1.end());
}

TEST(test_mpp_utils, std__vector_char_to_std__string) {
    std::vector<char> vec1 = {'h', 'e', 'l', 'l', 'o', 'w', 'o', 'r', 'l', 'd', '!'};
    std::string str1 = std::string(vec1.data());
}

class A {
public:
    int i = -1;

    A() : i(0) {};
    A(int i) : i(i) {};
};

template <typename T>
class B : public T {
public:
    B() : T() {};
    B(int i) : T(i) {};

    void set_a(T a) {
        (T&)*this = a;
    }

    T get_a() {
        return (T)*this;
    }
};

TEST(test_mpp_global_tests, set_inherited_class) {
    B b1 = B<A>(0);
    A a1 = A(1);
    b1.set_a(a1);

    EXPECT_EQ(1, b1.i);
}

TEST(test_mpp_global_tests, get_inherited_class) {
    B b1 = B<A>(0);
    A a1 = A(1);
    b1.set_a(a1);
    A a2 = b1.get_a();

    EXPECT_EQ(a1.i, a2.i);
}