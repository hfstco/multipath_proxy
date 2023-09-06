//
// Created by Matthias Hofstätter on 27.02.23.
// no inheritance! -> otherwise &this[0] = vptr and not ::sockaddr
// equal memory structure ::sockaddr _in _in6
// test_mpp_sockaddr ensures that
//

#ifndef MULTIPATH_PROXY_SOCKADDR_H
#define MULTIPATH_PROXY_SOCKADDR_H

#include <arpa/inet.h>
#include <cstring>
#include <sstream>

#include "../exception/Exception.h"

namespace net {

    struct SockAddr : public sockaddr {

        SockAddr() {
            memset(&this[0], 0, sizeof(struct sockaddr));
        };

        SockAddr(sa_family_t family) : SockAddr() {
            this->family(family);
        };

        sa_family_t family() {
            return this->sa_family;
        };

        void family(sa_family_t family) {
            this->sa_family = family;
        };

        std::string to_string() {
            std::stringstream stringStream;

            stringStream << "SA[family=";
            switch( family() ) {
                case AF_UNIX:
                    stringStream << "AF_UNIX";
                    break;
                case AF_INET:
                    stringStream << "AF_INET";
                    break;
                case AF_INET6:
                    stringStream << "AF_INET6";
                    break;
            }
            stringStream << "]";

            return stringStream.str();
        }
    };

    namespace ipv4 {

        struct SockAddr_In : public sockaddr_in {

            SockAddr_In() {
                memset(&this[0], 0, sizeof(struct sockaddr_in));
                this->family(AF_INET);
            };

            SockAddr_In(std::string ip, uint16_t port) : SockAddr_In() {
                this->ip(ip);
                this->port(port);
            };

            SockAddr_In(std::string& endpoint) : SockAddr_In() {
                size_t colon = endpoint.find(':');
                ip(endpoint.substr(0, colon));
                port(std::stoi(endpoint.substr(colon + 1, endpoint.length() - colon)));
            }

            SockAddr_In(in_addr sin6_addr, in_port_t port) : SockAddr_In() {
                this->sin_addr = sin6_addr;
                this->sin_port = port;
            };

            bool operator==(const SockAddr_In &other) const
            {
                return (this->sin_family == other.sin_family && this->sin_addr.s_addr == other.sin_addr.s_addr && this->sin_port == other.sin_port);
                //return memcmp(this, &other, sizeof(SockAddr_In)); // TODO check
            }

            sa_family_t family() {
                return this->sin_family;
            };

            std::string ip() {
                char ip[INET_ADDRSTRLEN];

                /*if (!inet_ntop(AF_INET, &sin_addr, ip, INET_ADDRSTRLEN)) {
                    throw SockAddrErrorException("inet_ntop() failed. errno=" + std::string(strerror(errno)));
                }*/

                int a,b,c,d;

                a = (0xFF000000 & sin_addr.s_addr) >> 24;
                b = (0x00FF0000 & sin_addr.s_addr) >> 16;
                c = (0x0000FF00 & sin_addr.s_addr) >> 8;
                d = 0x000000FF & sin_addr.s_addr;

                snprintf(ip,16,"%d.%d.%d.%d",d,c,b,a);

                return std::string(ip);
            }

            void ip(std::string ip) {
                int result;
                if ((result = inet_pton(AF_INET, ip.c_str(), &sin_addr)) <= 0) {
                    if (result == 0) {
                        throw SockAddrErrorException("not a valid ipv4 address.");
                    }
                    throw SockAddrErrorException("inet_pton() failed. errno=" + std::string(strerror(errno)));
                }
            }

            void ip(in_addr ip) {
                this->sin_addr = ip;
            }

            uint16_t port() {
                return ntohs(this->sin_port);
            }

            void port(uint16_t port) {
                this->sin_port = htons(port);
            }

            std::string to_string() {
                return ip() + ":" + std::to_string(port());
            }

        private:
            void family(sa_family_t family) {
                this->sin_family = family;
            };
        };
    }

    namespace ipv6 {

        struct SockAddr_In6 : public sockaddr_in6 {

            SockAddr_In6() {
                memset(&this[0], 0, sizeof(struct sockaddr_in6));
                this->family(AF_INET6);
            };

            SockAddr_In6(std::string ip, uint16_t port) : SockAddr_In6() {
                this->ip(ip);
                this->port(port);
            };

            SockAddr_In6(std::string endpoint) : SockAddr_In6() {
                int colon = endpoint.find(':');
                ip(endpoint.substr(0, colon));
                port(std::stoi(endpoint.substr(colon + 1, endpoint.length() - colon)));
            }

            SockAddr_In6(in6_addr sin6_addr, in_port_t port) : SockAddr_In6() {
                this->sin6_addr = sin6_addr;
                this->sin6_port = port;
            };

            bool operator==(const SockAddr_In6 &other) const
            {
                //return (this->sin6_family == other.sin6_family && this->sin6_addr == other.sin6_addr && this->sin6_port == other.sin6_port);
                return memcmp(this, &other, sizeof(SockAddr_In6)); // TODO check
            }

            sa_family_t family() {
                return this->sin6_family;
            };

            std::string ip() {
                char ip[INET6_ADDRSTRLEN];

                if (!inet_ntop(AF_INET6, &sin6_addr, ip, INET6_ADDRSTRLEN)) {
                    throw SockAddrErrorException("inet_ntop() failed. errno=" + std::string(strerror(errno)));
                }

                return std::string(ip);
            }

            void ip(std::string ip) {
                int result;
                if ((result = inet_pton(AF_INET6, ip.c_str(), &sin6_addr)) <= 0) {
                    if (result == 0) {
                        throw SockAddrErrorException("not a valid ipv6 address.");
                    }
                    throw SockAddrErrorException("inet_pton() failed. errno=" + std::string(strerror(errno)));
                }
            }

            unsigned short port() {
                return ntohs(this->sin6_port);
            }

            void port(uint16_t port) {
                this->sin6_port = htons(port);
            }

            std::string to_string() {
                return ip() + ":" + std::to_string(port());
            }

        private:
            void family(sa_family_t family) {
                this->sin6_family = family;
            };

            // TODO ipv6 members
        };
    }
}


#endif //MULTIPATH_PROXY_SOCKADDR_H
