//
// Created by Matthias Hofstätter on 22.02.23.
//

#include "gtest/gtest.h"
#include "../src/net/SockAddr.h"

TEST(net_SockAddr, memcmp_net_SockAddr) {
    net::SockAddr sa1 = net::SockAddr();
    struct ::sockaddr sa1_orig{};
    ::memset(&sa1_orig, 0, sizeof(struct sockaddr));
    EXPECT_EQ(0, std::memcmp(&sa1, &sa1_orig, sizeof(sockaddr)));
}

TEST(net_SockAddr, memcmp_net_ipv4_SockAddr) {
    net::ipv4::SockAddr_In sai1 = net::ipv4::SockAddr_In();
    struct ::sockaddr_in sai1_orig{};
    ::memset(&sai1_orig, 0, sizeof(struct ::sockaddr_in));
    sai1_orig.sin_family = AF_INET;
    EXPECT_EQ(0, std::memcmp(&sai1, &sai1_orig, sizeof(::sockaddr_in)));

    net::ipv4::SockAddr_In sai2 = net::ipv4::SockAddr_In(std::string("127.0.0.1"), 1234);
    struct ::sockaddr_in sai2_orig{};
    ::memset(&sai2_orig, 0, sizeof(struct ::sockaddr_in));
    sai2_orig.sin_family = AF_INET;
    sai2_orig.sin_port = htons(1234);
    ::inet_pton(AF_INET, std::string("127.0.0.1").c_str(), &sai2_orig.sin_addr);
    EXPECT_EQ(0, std::memcmp(&sai2, &sai2_orig, sizeof(::sockaddr_in)));
}

TEST(net_SockAddr, memcmp_net_ipv6_SockAddr) {
    net::ipv6::SockAddr_In6 sai61 = net::ipv6::SockAddr_In6();
    struct ::sockaddr_in6 sai61_orig{};
    ::memset(&sai61_orig, 0, sizeof(struct ::sockaddr_in6));
    sai61_orig.sin6_family = AF_INET6;
    EXPECT_EQ(0, std::memcmp(&sai61, &sai61_orig, sizeof(::sockaddr_in6)));

    net::ipv6::SockAddr_In6 sai62 = net::ipv6::SockAddr_In6(std::string("::1"), 1234);
    struct ::sockaddr_in6 sai62_orig {};
    ::memset(&sai62_orig, 0, sizeof(struct ::sockaddr_in6));
    sai62_orig.sin6_family = AF_INET6;
    sai62_orig.sin6_port = htons(1234);
    ::inet_pton(AF_INET6, std::string("::1").c_str(), &sai62_orig.sin6_addr);
    EXPECT_EQ(0, std::memcmp(&sai62, &sai62_orig, sizeof(::sockaddr_in6)));
}

TEST(net_SockAddr, net_ipv4_Sockaddr_get_set_ip) {
    std::string str1("127.0.0.1");
    net::ipv4::SockAddr_In sa1 = net::ipv4::SockAddr_In(str1, 1234);
    EXPECT_EQ(str1, sa1.ip());

    std::string s2("192.168.0.1");
    sa1.ip(s2);
    EXPECT_EQ(s2, sa1.ip());
}

TEST(net_SockAddr, net_ipv6_Sockaddr_get_set_ip) {
    std::string str1("::1");
    net::ipv6::SockAddr_In6 sa1 = net::ipv6::SockAddr_In6(str1, 1234);
    EXPECT_EQ(str1, sa1.ip());


    std::string s62("fd12:3456:789a:1::1");
    sa1.ip(s62);
    EXPECT_EQ(s62, sa1.ip());
}

TEST(net_SockAddr, net_ipv4_Sockaddr_get_set_port) {
    unsigned short p1 = 1234;
    unsigned short p2 = 4321;


    net::ipv4::SockAddr_In sa1 = net::ipv4::SockAddr_In("127.0.0.1", p1);
    EXPECT_EQ(p1, sa1.port());


    sa1.port(p2);
    EXPECT_EQ(p2, sa1.port());
}

TEST(net_SockAddr, net_ipv6_Sockaddr_get_set_port) {
    unsigned short p1 = 1234;
    unsigned short p2 = 4321;

    net::ipv6::SockAddr_In6 sa1 = net::ipv6::SockAddr_In6("::1", p1);
    EXPECT_EQ(p1, sa1.port());

    sa1.port(p2);
    EXPECT_EQ(p2, sa1.port());
}

TEST(net_SockAddr, net_ipv4_Sockaddr_get_family) {
    net::ipv4::SockAddr_In sa1 = net::ipv4::SockAddr_In("127.0.0.1", 2227);
    EXPECT_EQ(AF_INET, sa1.family());
}

TEST(net_SockAddr, net_ipv6_Sockaddr_get_family) {
    net::ipv6::SockAddr_In6 sa1 = net::ipv6::SockAddr_In6("::1", 2228);
    EXPECT_EQ(AF_INET6, sa1.family());
}
