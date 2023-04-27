//
// Created by Matthias Hofstätter on 17.04.23.
//

#ifndef MULTIPATH_PROXY_BUFFER_H
#define MULTIPATH_PROXY_BUFFER_H

#include <vector>
#include <string>

namespace packet {

    struct Buffer : private std::vector<unsigned char> { // TODO unsigned char*
        static Buffer *make(size_t size);
        static Buffer *make(unsigned char *data, size_t size);
        static Buffer *make(std::vector<unsigned char> data);

        unsigned char *data();

        uint16_t size();
        void resize(size_t size);

        std::string ToString();

        virtual ~Buffer();

    protected:
        Buffer(size_t size);
        Buffer(unsigned char *data, size_t size);
        Buffer(std::vector<unsigned char> data);

    };

} // packet

#endif //MULTIPATH_PROXY_BUFFER_H
