//
// Created by Matthias Hofstätter on 13.04.23.
//

#ifndef MULTIPATH_PROXY_HEADER_H
#define MULTIPATH_PROXY_HEADER_H

#include <string>

namespace packet {

        static const uint32_t MAGIC = 0xAAAAAAAA;

        enum TYPE : uint32_t {
            FLOW = 0xC001DA7A,
            HEARTBEAT = 0xC0DECAFE
        };

        struct __attribute__((packed)) Header {
        public:
            TYPE type() const;

            std::string ToString();

        protected:
            Header(TYPE type);

        private:
            uint32_t magic_ = MAGIC;     // 4
            TYPE type_;                  // 8
        };

} // packet

#endif //MULTIPATH_PROXY_HEADER_H
