//
// Created by Matthias Hofstätter on 22.08.23.
//

#ifndef MPP_BACKLOG_H
#define MPP_BACKLOG_H

#include <cstdint>
#include <mutex>
#include <atomic>

namespace backlog {

    class Chunk;

    class Backlog {
    public:
        Backlog() = default;

        explicit operator uint64_t() const noexcept
        { return _size; }

        explicit operator uint64_t() const volatile noexcept
        { return _size; }

        friend std::ostream& operator<<(std::ostream& ostream, const Backlog& backlog)
        {
            ostream << std::to_string(backlog._size);
            return ostream;
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

        uint64_t size() {
            return _size;
        };

        bool empty() {
            return _size == 0;
        };

        virtual void reset() = 0;

        virtual void insert(Chunk *chunk) = 0;
        virtual Chunk *next(uint64_t max) = 0;

    protected:
        std::mutex _mutex;

        std::atomic<uint64_t> _size = 0;
        std::atomic<uint64_t> _offset = 0;
    };

} // backlog

#endif //MPP_BACKLOG_H
