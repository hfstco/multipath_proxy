//
// Created by Matthias Hofstätter on 13.04.23.
//

#ifndef MULTIPATH_PROXY_HEADER_H
#define MULTIPATH_PROXY_HEADER_H

#include <string>

namespace packet {

        static const uint16_t MAGIC = 0xAAAA;

        enum TYPE : uint16_t {
            FLOW = 0xDA7A,
            HEARTBEAT = 0xBEA7
        };

        struct __attribute__((packed)) Header {
        public:
            TYPE type() const;

            std::string ToString();

        protected:
            Header(TYPE type);

        private:
            uint16_t magic_ = MAGIC;     // 2
            TYPE type_;                  // 4
        };

} // packet

#endif //MULTIPATH_PROXY_HEADER_H
