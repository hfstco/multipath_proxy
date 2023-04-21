//
// Created by Matthias Hofstätter on 17.04.23.
//

#ifndef MULTIPATH_PROXY_BUFFER_H
#define MULTIPATH_PROXY_BUFFER_H

#include <vector>

namespace packet {

    struct Buffer : private std::vector<unsigned char> {
        static Buffer *make(size_t size) {
            return new Buffer(size);
        }

        static Buffer *make(unsigned char *data, size_t size) {
            return new Buffer(data, size);
        }

        static Buffer *make(std::vector<unsigned char> data) {
            return new Buffer(data);
        }

        virtual unsigned char *data() {
            return std::vector<unsigned char>::data();
        }

        uint16_t size() {
            return std::vector<unsigned char>::size();
        }

        void resize(size_t size) {
            std::vector<unsigned char>::resize(size);
        }

    protected:
        Buffer(size_t size) : std::vector<unsigned char>(size) {}

        Buffer(unsigned char *data, size_t size) : std::vector<unsigned char>(data, data + size) {}

        Buffer(std::vector<unsigned char> data) : std::vector<unsigned char>(data.data(),
                                                                             data.data() + data.size()) {}

    };

} // packet

#endif //MULTIPATH_PROXY_BUFFER_H
