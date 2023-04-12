//
// Created by Matthias Hofstätter on 04.04.23.
//

#include "TcpListener.h"


namespace net {
    namespace ipv4 {

        TcpConnection TcpListener::Accept() {
            return Listener<TcpSocket, SockAddr_In>::Accept();
        }

    } // ipv4

    namespace ipv6 {

        TcpConnection TcpListener::Accept() {
            return Listener<TcpSocket, SockAddr_In6>::Accept();
        }

    }

} // net

