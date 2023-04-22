//
// Created by Matthias Hofstätter on 27.02.23.
// no inheritance! -> otherwise &this[0] = vptr and not ::sockaddr
// equal memory structure ::sockaddr _in _in6
// test_mpp_sockaddr ensures that
//

#ifndef MULTIPATH_PROXY_SOCKADDR_H
#define MULTIPATH_PROXY_SOCKADDR_H

#include <arpa/inet.h>
#include <string>
#include "../../exception/Exception.h"
#include "../../utils/Utils.h"

namespace net {

    struct SockAddr : public sockaddr {

        SockAddr() {
            memset(&this[0], 0, sizeof(struct sockaddr));
        };

        SockAddr(sa_family_t family) : SockAddr() {
            this->family(family);
        };

        socklen_t length() {
            return this->sa_len;
        }

        sa_family_t family() {
            return this->sa_family;
        };

        void family(sa_family_t family) {
            this->sa_family = family;
        };
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

            SockAddr_In(in_addr sin6_addr, in_port_t port) : SockAddr_In() {
                this->sin_addr = sin6_addr;
                this->sin_port = port;
            };

            socklen_t length() {
                return this->sin_len;
            }

            sa_family_t family() {
                return this->sin_family;
            };

            std::string ip() {
                try {
                    return utils::InAddrToString(this->sin_addr);
                } catch (UtilsException e) {
                    throw SockAddrErrorException(e.what());
                }
            }

            void ip(std::string ip) {
                try {
                    this->sin_addr = utils::StringToInAddr(ip);
                } catch (UtilsException e) {
                    throw SockAddrErrorException(e.what());
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

            SockAddr_In6(in6_addr sin6_addr, in_port_t port) : SockAddr_In6() {
                this->sin6_addr = sin6_addr;
                this->sin6_port = port;
            };

            socklen_t length() {
                return this->sin6_len;
            }

            sa_family_t family() {
                return this->sin6_family;
            };

            std::string ip() {
                try {
                    return utils::In6AddrToString(this->sin6_addr);
                } catch (UtilsException e) {
                    throw SockAddrErrorException(e.what());
                }
            }

            void ip(std::string ip) {
                try {
                    this->sin6_addr = utils::StringToIn6Addr(ip);
                } catch (UtilsException e) {
                    throw SockAddrErrorException(e.what());
                }
            }

            unsigned short port() {
                return ntohs(this->sin6_port);
            }

            void port(uint16_t port) {
                this->sin6_port = htons(port);
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
