//
// Created by Matthias Hofstätter on 02.09.23.
//

#include <gtest/gtest.h>
#include <glog/logging.h>
#include "../src/backlog/TestChunk.h"

TEST(chunk_tests, simple_chunk) {
    TestChunk *chunk = new TestChunk;
    auto length = chunk->length();
    auto offset = chunk->offset();
    chunk->offset(std::numeric_limits<uint64_t>::max());
    chunk->length(std::numeric_limits<uint64_t>::max());
    chunk->offset(0);
    chunk->length(0);
length = chunk->length();
}