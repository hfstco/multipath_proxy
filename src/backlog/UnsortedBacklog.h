//
// Created by Matthias Hofstätter on 20.08.23.
//

#ifndef MPP_UNSORTEDBACKLOG_H
#define MPP_UNSORTEDBACKLOG_H

#include <cstdint>
#include <queue>
#include <mutex>
#include <atomic>

#include "Backlog.h"

namespace backlog {

    class Chunk;

    class UnsortedBacklog : public Backlog {
    public:
        UnsortedBacklog();

        void reset() override;

        void insert(Chunk *chunk) override;
        Chunk *next() override;

        virtual ~UnsortedBacklog();

    private:
        std::queue<Chunk *> _chunks;

        // TODO add source, dest and compare on insert
    };

} // backlog

#endif //MPP_UNSORTEDBACKLOG_H
