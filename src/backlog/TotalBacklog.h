//
// Created by Matthias Hofstätter on 22.08.23.
//

#ifndef MPP_TOTALBACKLOG_H
#define MPP_TOTALBACKLOG_H

#include <cstdint>
#include <atomic>
#include <iostream>

namespace backlog {

    class Backlog;

    // singleton pattern
    // https://stackoverflow.com/questions/1008019/c-singleton-design-pattern
    class TotalBacklog {
        friend class Backlog;
    public:
        explicit operator uint64_t() const noexcept
        { return _size; }

        explicit operator uint64_t() const volatile noexcept
        { return _size; }

        friend std::ostream& operator<<(std::ostream& os, const TotalBacklog& dt)
        {
            os << std::to_string(dt._size);
            return os;
        }

        friend bool operator== (const TotalBacklog &backlog, const uint64_t &value)
        {
            return backlog._size == value;
        }

        friend bool operator!= (const TotalBacklog &backlog, const uint64_t &value)
        {
            return backlog._size != value;
        }

        friend bool operator> (const TotalBacklog &backlog, const uint64_t &value)
        {
            return backlog._size > value;
        }

        friend bool operator< (const TotalBacklog &backlog, const uint64_t &value)
        {
            return backlog._size < value;
        }

        friend bool operator<= (const TotalBacklog &backlog, const uint64_t &value)
        {
            return backlog._size <= value;
        }

        friend bool operator>= (const TotalBacklog &backlog, const uint64_t &value)
        {
            return backlog._size >= value;
        }

        uint64_t operator+=(uint64_t size) noexcept
        { return _size += size; }

        uint64_t operator-=(uint64_t size) volatile noexcept
        { return _size -= size; }

        static TotalBacklog& get()
        {
            static TotalBacklog threadpool; // Guaranteed to be destroyed.
            // Instantiated on first use.
            return threadpool;
        }

        // C++ 11
        // =======
        // We can use the better technique of deleting the methods
        // we don't want.
        TotalBacklog(TotalBacklog const&) = delete;
        void operator=(TotalBacklog const&) = delete;

        // Note: Scott Meyers mentions in his Effective Modern
        //       C++ book, that deleted functions should generally
        //       be public as it results in better error messages
        //       due to the compilers behavior to check accessibility
        //       before deleted status
    private:
        std::atomic<uint64_t> _size = 0;

        TotalBacklog() = default; // Constructor? (the {} brackets) are needed here.
    };

} // backlog

#define TOTAL_BACKLOG backlog::TotalBacklog::get()

#endif //MPP_TOTALBACKLOG_H
