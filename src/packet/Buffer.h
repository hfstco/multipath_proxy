//
// Created by Matthias Hofstätter on 17.04.23.
//

#ifndef MULTIPATH_PROXY_BUFFER_H
#define MULTIPATH_PROXY_BUFFER_H

#include <vector>
#include <string>

namespace packet {

    struct Buffer { // TODO unsigned char*
        Buffer() = delete;

        static Buffer *make(uint16_t size);
        static Buffer *make(unsigned char *data, uint16_t size);
        static Buffer *make(std::vector<unsigned char> data);

        unsigned char *data();
        uint16_t size();

        void Resize(uint16_t size);

        std::string ToString();

        virtual ~Buffer();

    protected:
        Buffer(uint16_t size);
        Buffer(unsigned char *data, uint16_t size);
        Buffer(std::vector<unsigned char> data);

    private:
        uint16_t size_;
        unsigned char *data_;
    };

} // packet

#endif //MULTIPATH_PROXY_BUFFER_H
