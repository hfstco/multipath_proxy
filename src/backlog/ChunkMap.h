//
// Created by Matthias Hofstätter on 03.08.23.
//

#ifndef MPP_CHUNKMAP_H
#define MPP_CHUNKMAP_H

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

        std::string ToString() {
            return "Chunk[offset=" + std::to_string(offset) + ", size=" + std::to_string(size) + "]";
        }

        virtual ~Chunk() {
            if (data) {
                free(data);
            }
        }
    };

    class ChunkMap {
    public:
        ChunkMap();

        const uint64_t size();
        const uint64_t offset();
        const uint64_t available();

        bool Empty();
        void Reset();

        virtual void Insert(Chunk *chunk);
        virtual Chunk *Next(uint64_t max);

        virtual ~ChunkMap();

    private:
        std::map<uint64_t, Chunk *> _chunks;
        std::mutex _mutex;

        std::atomic<uint64_t> _size;
        std::atomic<uint64_t> _offset;
        std::atomic<uint64_t> _available;

        // TODO add source, dest and compare on insert
    };

} // backlog

#endif //MPP_CHUNKMAP_H
