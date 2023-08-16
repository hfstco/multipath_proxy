//
// Created by Matthias Hofstätter on 07.08.23.
//

#include <gtest/gtest.h>
#include <glog/logging.h>
#include <list>

#include "../src/backlog/Backlog.h"

TEST(backlog, insert_seqential) {
    uint64_t offset = 0;

    std::string string("HelloWorldHelloWorldHelloWorldHelloWorldHelloWorldHelloWorldHelloWorldHelloWorldHelloWorldHelloWorldHelloWorldHelloWorldHelloWorldHelloWorldHelloWorldHelloWorldHelloWorldHelloWorldHelloWorldHelloWorldHelloWorldHelloWorldHelloWorldHelloWorld");


    backlog::Backlog backlog = backlog::Backlog(true);

    backlog::Chunk *chunk;
    uint64_t count = 0;
    while(offset < 50) {
        // rnd chunk size
        srand(time(NULL));
        uint64_t size = rand() % 15 + 5;

        // create chunk
        chunk = new backlog::Chunk();
        chunk->data = static_cast<unsigned char *>(malloc(size));
        memcpy(chunk->data, string.data() + offset, size);
        chunk->offset = offset;
        chunk->size = size;

        // insert into backlog
        backlog.Insert(chunk);

        offset += size;
        count++;
    }

    CHECK(backlog.size() == offset);
    CHECK(backlog::TotalBacklog == offset);

    delete backlog.Next(10);
    delete backlog.Next(10);
    delete backlog.Next(10);
    delete backlog.Next(10);
    delete backlog.Next(10);
    delete backlog.Next(10);
    delete backlog.Next(10);

    backlog.Reset();
}

TEST(backlog, insert_random) {
    uint64_t offset = 0;

    std::string string("HelloWorldHelloWorldHelloWorldHelloWorldHelloWorldHelloWorldHelloWorldHelloWorldHelloWorldHelloWorldHelloWorldHelloWorldHelloWorldHelloWorldHelloWorldHelloWorldHelloWorldHelloWorldHelloWorldHelloWorldHelloWorldHelloWorldHelloWorldHelloWorld");
    std::list<backlog::Chunk *> chunks;

    backlog::Backlog backlog = backlog::Backlog(true);

    backlog::Chunk *chunk;
    uint64_t count = 0;
    while(offset < 100) {
        // rnd chunk size
        srand(time(NULL));
        uint64_t size = rand() % 15 + 5;

        // create chunk
        chunk = new backlog::Chunk();
        chunk->data = static_cast<unsigned char *>(malloc(size));
        memcpy(chunk->data, string.data() + offset, size);
        chunk->offset = offset;
        chunk->size = size;

        // insert into backlog
        chunks.push_back(chunk);

        offset += size;
        count++;
    }

    while(!chunks.empty()) {
        auto it = chunks.cbegin();
        int random = rand() % chunks.size();
        std::advance(it, random);

        backlog.Insert((*it));

        chunks.erase(it);
    }

    CHECK(backlog.size() == offset);
    CHECK(backlog::TotalBacklog == offset);

    backlog::Chunk *chunk1;
    uint64_t off = 0;
    unsigned char *buf = static_cast<unsigned char *>(malloc(offset));
    while ((chunk1 = backlog.Next((rand() % 5 + 15))) != nullptr) {
        memcpy(buf + off, chunk1->data, chunk1->size);
        off += chunk1->size;
    }

    CHECK(memcmp(buf, string.data(), offset) == 0);

    backlog.Reset();
}