//
// Created by Matthias Hofstätter on 25.04.23.
//

#include <glog/logging.h>

#include "Buffer.h"

namespace packet {

    Buffer::Buffer(size_t size) : std::vector<unsigned char>(size) {}

    Buffer::Buffer(unsigned char *data, size_t size) : std::vector<unsigned char>(data, data + size) {}

    Buffer::Buffer(std::vector<unsigned char> data) : std::vector<unsigned char>(data.data(),
            data.data() + data.size()) {}

    Buffer *Buffer::make(size_t size) {
        return new Buffer(size);
    }

    Buffer *Buffer::make(unsigned char *data, size_t size) {
        return new Buffer(data, size);
    }

    Buffer *Buffer::make(std::vector<unsigned char> data) {
        return new Buffer(data);
    }

    unsigned char *Buffer::data() {
        return std::vector<unsigned char>::data();
    }

    uint16_t Buffer::size() {
        return std::vector<unsigned char>::size();
    }

    void Buffer::resize(size_t size) {
        std::vector<unsigned char>::resize(size);
    }

    std::string Buffer::ToString() {
        return std::string("B[size=" + std::to_string(size()) + "]");
    }

    Buffer::~Buffer() {
        LOG(INFO) << "del " + ToString();
    }

} // packet
