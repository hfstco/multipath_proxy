//
// Created by Matthias Hofstätter on 31.07.23.
//

#include <glog/logging.h>
#include "gtest/gtest.h"

#include "../src/net/QuicServerConnection.h"
#include "../src/net/QuicClientConnection.h"
#include "../src/net/QuicStream.h"

TEST(picoquic, simple) {
    net::QuicServerConnection qsc = net::QuicServerConnection(7779);

    net::QuicClientConnection qcc = net::QuicClientConnection("localhost", 7779);

    net::QuicStream *qs = qcc.CreateStream();
    uint8_t *buffer = (uint8_t *)"Hello World!";
    qs->Send(buffer, 5);
}