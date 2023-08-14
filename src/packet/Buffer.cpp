//
// Created by Matthias Hofstätter on 25.04.23.
//

#include <glog/logging.h>

#include <utility>

#include "Buffer.h"
#include "../exception/Exception.h"

namespace packet {

    Buffer::Buffer(uint16_t size) : data_((unsigned char *)malloc(size)), size_(size) {} // TODO malloc?

    Buffer::Buffer(unsigned char *data, uint16_t size) : Buffer(size) {
        memcpy(data_, data, size);
    }

    Buffer::Buffer(std::vector<unsigned char> data) : Buffer(data.data(), data.size()) {}

    Buffer *Buffer::make(uint16_t size) {
        return new Buffer(size);
    }

    Buffer *Buffer::make(unsigned char *data, uint16_t size) {
        return new Buffer(data, size);
    }

    Buffer *Buffer::make(std::vector<unsigned char> data) {
        return new Buffer(std::move(data));
    }

    unsigned char *Buffer::data() {
        return (unsigned char *)data_;
    }

    uint16_t Buffer::size() {
        return size_;
    }

    void Buffer::Resize(uint16_t size) {
        auto *newDataPtr = (unsigned char *)realloc(data_, size);
        if( !newDataPtr ) {
            BufferReallocError bufferReallocError = BufferReallocError("Can't realloc(data_, " + std::to_string(size) + ".");

            LOG(INFO) << ToString() << " ~> " << bufferReallocError.ToString();

            throw bufferReallocError;
        }
        data_ = newDataPtr;
        size_ = size;
    }

    std::string Buffer::ToString() {
        return std::string("Buffer[Size=" + std::to_string(size()) + "]");
    }

    Buffer::~Buffer() {
        //LOG(INFO) << "~" << ToString();
        free(data_);
    }

} // packet
