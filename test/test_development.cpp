//
// Created by Matthias Hofstätter on 24.02.23.
//

#include <gtest/gtest.h>
#include <glog/logging.h>
//#include <uuid/uuid.h>

TEST(development_tests, std_string_to_std_vector_char) {
    std::string str1("helloworld!");
    std::vector<char> vec1 = {'h', 'e', 'l', 'l', 'o', 'w', 'o', 'r', 'l', 'd', '!'};

    std::vector<char> vec2(str1.begin(), str1.end());

    EXPECT_EQ(0, strcmp(vec1.data(), vec2.data()));
}

TEST(development_tests, std__vector_char_to_std__string) {
    std::string str1("helloworld!");
    std::vector<char> vec1 = {'h', 'e', 'l', 'l', 'o', 'w', 'o', 'r', 'l', 'd', '!'};

    std::string str2 = std::string(vec1.data());

    EXPECT_EQ(0, strcmp(str2.c_str(), vec1.data()));
}

struct X {
    int a;
    int b;
    short c;
};

TEST(development_tests, struct_to_buffer_vector) {
    X x = X();
    x.a = 1;
    x.b = 2;
    x.c = 3;

    auto const ptr = reinterpret_cast<unsigned char *>(&x);
    std::vector<unsigned char> b1 = std::vector<unsigned char>(ptr, ptr + sizeof(X));
}

class A {
public:
    int i_ = -1;

    A() : i_(0) {};
    A(int i) : i_(i) {};
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

TEST(development_tests, set_inherited_class) {
    B b1 = B<A>(0);
    A a1 = A(1);
    b1.set_a(a1);

    EXPECT_EQ(1, b1.i_);
}

TEST(development_tests, get_inherited_class) {
    B b1 = B<A>(0);
    A a1 = A(1);
    b1.set_a(a1);
    A a2 = b1.get_a();

    EXPECT_EQ(a1.i_, a2.i_);
}

/*
TEST(development_tests, uuid) {
    uuid_t uuid;
    uuid_generate(uuid);

    LOG(INFO) << sizeof(uuid);
}*/
