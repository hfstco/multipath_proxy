//
// Created by Matthias Hofstätter on 31.07.23.
//

#include <glog/logging.h>
#include "gtest/gtest.h"

#include "../src/net/QuicServerConnection.h"
#include "../src/net/QuicClientConnection.h"
#include "../src/quic/Stream.h"

TEST(picoquic, simple) {
    net::QuicServerConnection qsc = net::QuicServerConnection(7779, false);

    //net::QuicClientConnection qcc = net::QuicClientConnection("localhost", 7779, false, "ticket_store.bin", "token_store.bin");
}