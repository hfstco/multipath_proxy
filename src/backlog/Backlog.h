//
// Created by Matthias Hofstätter on 20.08.23.
//

#ifndef MPP_BACKLOG_H
#define MPP_BACKLOG_H

#include "ChunkMap.h"

namespace backlog {

    class Backlog : public ChunkMap {
    public:
        void Insert(Chunk *chunk) override;
        Chunk *Next(uint64_t max) override;

        ~Backlog() override;
    };

    class TotalBacklog {
        friend class Backlog;
    public:
        static uint64_t size();

    private:
        inline static std::atomic<uint64_t> _size = 0;
    };

} // backlog

#endif //MPP_BACKLOG_H
