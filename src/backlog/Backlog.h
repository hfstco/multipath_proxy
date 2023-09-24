//
// Created by Matthias Hofstätter on 06.09.23.
//

#ifndef MPP_BACKLOG_H
#define MPP_BACKLOG_H

#include "ChunkQueue.h"

namespace backlog {

    class Chunk;

    class Backlog : public ChunkQueue {
    public:
        explicit operator uint64_t() const noexcept
        { return _size; }

        explicit operator uint64_t() const volatile noexcept
        { return _size; }

        friend std::ostream& operator<<(std::ostream& os, const Backlog& dt)
        {
            os << std::to_string(dt._size);
            return os;
        }

        friend bool operator== (const Backlog &backlog, const uint64_t &value)
        {
            return backlog._size == value;
        }

        friend bool operator!= (const Backlog &backlog, const uint64_t &value)
        {
            return backlog._size != value;
        }

        friend bool operator> (const Backlog &backlog, const uint64_t &value)
        {
            return backlog._size > value;
        }

        friend bool operator< (const Backlog &backlog, const uint64_t &value)
        {
            return backlog._size < value;
        }

        friend bool operator<= (const Backlog &backlog, const uint64_t &value)
        {
            return backlog._size <= value;
        }

        friend bool operator>= (const Backlog &backlog, const uint64_t &value)
        {
            return backlog._size >= value;
        }

        Backlog();

        uint64_t size();

        void clear() override;
        void push(Chunk *chunk) override;
        Chunk *pop() override;

        ~Backlog() override;

    private:
        uint64_t _size;
    };

} // backlog

#endif //MPP_BACKLOG_H
