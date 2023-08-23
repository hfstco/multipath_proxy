//
// Created by Matthias Hofstätter on 22.08.23.
//

#ifndef MPP_TOTALBACKLOG_H
#define MPP_TOTALBACKLOG_H

#include <cstdint>
#include <atomic>
#include <iostream>

namespace backlog {

    class TotalBacklog {
        friend class UnsortedBacklog;
    public:
        explicit operator uint64_t() const noexcept
        { return _size; }

        explicit operator uint64_t() const volatile noexcept
        { return _size; }

        friend std::ostream& operator<<(std::ostream& os, const TotalBacklog& dt)
        {
            os << std::to_string(backlog::TotalBacklog::_size);
            return os;
        }

        friend bool operator== (const TotalBacklog &backlog, const uint64_t &value)
        {
            return backlog::TotalBacklog::_size == value;
        }

        friend bool operator!= (const TotalBacklog &backlog, const uint64_t &value)
        {
            return backlog::TotalBacklog::_size != value;
        }

        friend bool operator> (const TotalBacklog &backlog, const uint64_t &value)
        {
            return backlog::TotalBacklog::_size > value;
        }

        friend bool operator< (const TotalBacklog &backlog, const uint64_t &value)
        {
            return backlog::TotalBacklog::_size < value;
        }

        friend bool operator<= (const TotalBacklog &backlog, const uint64_t &value)
        {
            return backlog::TotalBacklog::_size <= value;
        }

        friend bool operator>= (const TotalBacklog &backlog, const uint64_t &value)
        {
            return backlog::TotalBacklog::_size >= value;
        }

        static uint64_t size() {
            return _size;
        }

    private:
        inline static std::atomic<uint64_t> _size = 0;

        uint64_t operator+=(uint64_t size) const noexcept
        { return _size.fetch_add(size); }

        uint64_t operator-=(uint64_t size) const volatile noexcept
        { return _size.fetch_sub(size); }
    };

    static inline TotalBacklog total_backlog;

} // backlog

#endif //MPP_TOTALBACKLOG_H
