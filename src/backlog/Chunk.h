//
// Created by Matthias Hofstätter on 22.08.23.
//

#ifndef MPP_CHUNK_H
#define MPP_CHUNK_H

#include <cstdint>
#include <string>

namespace backlog {

    struct ChunkHeader {
        uint64_t offset = 0;
        uint64_t size = 0;

        [[nodiscard]] std::string to_string() const {
            return "ChunkHeader[offset=" + std::to_string(offset) + ", size=" + std::to_string(size) + "]";
        }
    };

    struct Chunk {
        ChunkHeader header;
        uint8_t *data;

        [[nodiscard]] std::string to_string() const {
            return "Chunk[offset=" + std::to_string(header.offset) + ", size=" + std::to_string(header.size) + "]";
        }
    };

    /*struct Chunk {
    public:
        Chunk() : _data(static_cast<uint8_t *>(malloc(2 * sizeof(uint64_t)))) {}
        explicit Chunk(uint64_t size) : _data(static_cast<uint8_t *>(malloc(size + 2 * sizeof(uint64_t)))) {

        }
        Chunk(uint64_t offset, uint64_t size) : _data(static_cast<uint8_t *>(malloc(size + 2 * sizeof(uint64_t)))) {
            this->offset(offset);
            this->size(size);
        }

        [[nodiscard]] uint64_t offset() const {
            return *_data;
        }

        void offset(uint64_t offset) const {
            memcpy(_data, &offset, sizeof(uint64_t));
        }

        [[nodiscard]] uint64_t size() const {
            return *(_data + sizeof(uint64_t));
        }

        void size(uint64_t size) const {
            memcpy(_data + sizeof(uint64_t), &size, sizeof(uint64_t));
        }

        [[nodiscard]] uint8_t *payload() const {
            return (_data + (2 * sizeof(uint64_t)));
        }

        uint8_t *data() const {
            return _data;
        }

        [[nodiscard]] std::string to_string() const {
            return "Chunk[offset=" + std::to_string(offset()) + ", size=" + std::to_string(size()) + "]";
        }

        virtual ~Chunk() {
            if (_data) {
                free(_data);
            }
        }

    private:
        uint8_t *_data = nullptr;

        void resize(uint64_t size) {
            // force size >= 2 * sizeof(uint64_t)
            size = std::max(size, 2 * sizeof(uint64_t));

            // alloc new data buffer
            uint8_t *buffer = static_cast<uint8_t *>(malloc(size));

            memcpy(buffer, _data, std::min(size, this->size()));
            delete _data;
            _data = buffer;

            this->size(size);
        }
    };*/

} // backlog

#endif //MPP_CHUNK_H
