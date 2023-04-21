//
// Created by Matthias Hofstätter on 13.04.23.
//

#ifndef MULTIPATH_PROXY_HEADER_H
#define MULTIPATH_PROXY_HEADER_H

namespace packet {
    namespace header {

        static const uint32_t MAGIC = 0xDEADBEEF;

        enum TYPE : uint32_t {
            FLOW = 0xC001DA7A,
            HEARTBEAT = 0xC0DECAFE
        };

        struct __attribute__((packed)) Header {
        public:
            TYPE type() const {
                return type_;
            }

        protected:
            Header(TYPE type) : type_(type) {}

        private:
            uint32_t magic_ = MAGIC;     // 4
            TYPE type_;                  // 8
        };

    } // header
} // packet

#endif //MULTIPATH_PROXY_HEADER_H
