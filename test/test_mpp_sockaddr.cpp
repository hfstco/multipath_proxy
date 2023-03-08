//
// Created by Matthias Hofstätter on 22.02.23.
//

#include "gtest/gtest.h"
#include "../src/net/SockAddr.h"

TEST(test_mpp_sockaddr, __sockaddr_memcomp_mpp__sockaddr) {
    mpp::net::SockAddr sa1 = mpp::net::SockAddr();
    struct ::sockaddr sa1_orig {};
    ::memset(&sa1_orig, 0, sizeof(struct ::sockaddr));
    EXPECT_EQ(0, std::memcmp(&sa1, &sa1_orig, sizeof(::sockaddr)));


    mpp::net::ipv4::SockAddr_In sai1 = mpp::net::ipv4::SockAddr_In();
    struct ::sockaddr_in sai1_orig {};
    ::memset(&sai1_orig, 0, sizeof(struct ::sockaddr_in));
    sai1_orig.sin_family = AF_INET;
    EXPECT_EQ(0, std::memcmp(&sai1, &sai1_orig, sizeof(::sockaddr_in)));


    mpp::net::ipv6::SockAddr_In6 sai61 = mpp::net::ipv6::SockAddr_In6();
    struct ::sockaddr_in6 sai61_orig {};
    ::memset(&sai61_orig, 0, sizeof(struct ::sockaddr_in6));
    sai61_orig.sin6_family = AF_INET6;
    EXPECT_EQ(0, std::memcmp(&sai61, &sai61_orig, sizeof(::sockaddr_in6)));


    mpp::net::ipv4::SockAddr_In sai2 = mpp::net::ipv4::SockAddr_In(std::string("127.0.0.1"), 1234);
    struct ::sockaddr_in sai2_orig {};
    ::memset(&sai2_orig, 0, sizeof(struct ::sockaddr_in));
    sai2_orig.sin_family = AF_INET;
    sai2_orig.sin_port = htons(1234);
    ::inet_pton(AF_INET, std::string("127.0.0.1").c_str(), &sai2_orig.sin_addr);
    EXPECT_EQ(0, std::memcmp(&sai2, &sai2_orig, sizeof(::sockaddr_in)));


    mpp::net::ipv6::SockAddr_In6 sai62 = mpp::net::ipv6::SockAddr_In6(std::string("::1"), 1234);
    struct ::sockaddr_in6 sai62_orig {};
    ::memset(&sai2_orig, 0, sizeof(struct ::sockaddr_in6));
    sai62_orig.sin6_family = AF_INET6;
    sai62_orig.sin6_port = htons(1234);
    ::inet_pton(AF_INET6, std::string("::1").c_str(), &sai62_orig.sin6_addr);
    EXPECT_EQ(0, std::memcmp(&sai62, &sai62_orig, sizeof(::sockaddr_in6)));
}

TEST(test_mpp_sockaddr, mpp__sockaddr_get_set_ip) {
    std::string s1("127.0.0.1");
    mpp::net::ipv4::SockAddr_In sai2 = mpp::net::ipv4::SockAddr_In(s1, 1234);
    EXPECT_EQ(s1, sai2.ip());


    std::string s2("192.168.0.1");
    sai2.ip(s2);
    EXPECT_EQ(s2, sai2.ip());


    std::string s61("::1");
    mpp::net::ipv6::SockAddr_In6 sai61 = mpp::net::ipv6::SockAddr_In6(s61, 1234);
    EXPECT_EQ(s61, sai61.ip());


    std::string s62("fd12:3456:789a:1::1");
    sai61.ip(s62);
    EXPECT_EQ(s62, sai61.ip());
}

TEST(test_mpp_sockaddr, mpp__sockaddr_get_set_port) {
    unsigned short p1 = 1234;
    unsigned short p2 = 4321;


    mpp::net::ipv4::SockAddr_In sai2 = mpp::net::ipv4::SockAddr_In("127.0.0.1", p1);
    EXPECT_EQ(p1, sai2.port());


    sai2.port(p2);
    EXPECT_EQ(p2, sai2.port());


    mpp::net::ipv6::SockAddr_In6 sai61 = mpp::net::ipv6::SockAddr_In6("::1", p1);
    EXPECT_EQ(p1, sai61.port());

    sai61.port(p2);
    EXPECT_EQ(p2, sai61.port());
}
