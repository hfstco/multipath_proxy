//
// Created by Matthias Hofstätter on 20.08.23.
//

#ifndef MPP_CHUNKQUEUE_H
#define MPP_CHUNKQUEUE_H

#include <cstdint>
#include <queue>
#include <mutex>
#include <atomic>

namespace backlog {

    class Chunk;

    class ChunkQueue {
    public:
        ChunkQueue();

        virtual void clear();

        virtual void push(Chunk *chunk);
        virtual Chunk *pop();

        virtual ~ChunkQueue();

    protected:
        std::mutex _mutex;
        std::queue<Chunk *> _chunks;

        void _clear();

        void _push(Chunk *chunk);
        Chunk *_pop();

        // TODO add source, dest and compare on insert
    };

} // backlog

#endif //MPP_CHUNKQUEUE_H
