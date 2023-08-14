//
// Created by Matthias Hofstätter on 03.08.23.
//

#ifndef MPP_BACKLOG_H
#define MPP_BACKLOG_H

#include <cstdint>
#include <mutex>
#include <map>
#include <atomic>

namespace backlog {

    struct Chunk {
        unsigned char *data;
        uint64_t size;
        uint64_t offset;

        Chunk() :   data(),
                    size(0),
                    offset(0) {
        }

        Chunk(unsigned char *data, uint64_t size, uint64_t offset) :    data(data),
                                                                        size(size),
                                                                        offset(offset) {
        }

        std::string ToString() {
            return "Chunk[offset=" + std::to_string(offset) + ", size=" + std::to_string(size) + "]";
        }

        virtual ~Chunk() {
            if (data) {
                free(data);
            }
        }
    };

    class Backlog {
    public:
        explicit Backlog(bool is_rx); // outgoing backlogs do not affect totalBacklog
        Backlog();

        uint64_t size();
        uint64_t offset();
        uint64_t available();

        bool Empty();
        void Reset();

        void Insert(Chunk *chunk);
        Chunk *Next(uint64_t max = std::numeric_limits<uint64_t>::max());

        virtual ~Backlog();

    private:
        std::map<uint64_t, Chunk *> _chunks;
        std::mutex _mutex;

        std::atomic<uint64_t> _size = 0;
        std::atomic<uint64_t> _offset = 0;
        std::atomic<uint64_t> _available = 0;
        const bool _is_rx = false;

        // TODO add source, dest and compare on insert
    };

    inline std::atomic<uint64_t> TotalBacklog = 0;

} // backlog

#endif //MPP_BACKLOG_H
