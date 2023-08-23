//
// Created by Matthias Hofstätter on 03.08.23.
//

#ifndef MPP_SORTEDBACKLOG_H
#define MPP_SORTEDBACKLOG_H

#include <cstdint>
#include <mutex>
#include <map>
#include <atomic>

#include "Backlog.h"

namespace backlog {

    class Chunk;

    class SortedBacklog : public Backlog {
    public:
        SortedBacklog();

        void reset() override;

        void insert(Chunk *chunk) override;
        Chunk *next(uint64_t max) override;

        virtual ~SortedBacklog();

    private:
        std::map<uint64_t, Chunk *> _chunks;

        std::atomic<uint64_t> _total;

        // TODO add source, dest and compare on insert
    };

} // backlog

#endif //MPP_SORTEDBACKLOG_H
