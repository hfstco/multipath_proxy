//
// Created by Matthias Hofstätter on 05.03.23.
//

#ifndef MULTIPATH_PROXY_MESSAGE_H
#define MULTIPATH_PROXY_MESSAGE_H

#include <cstddef>
#include <vector>


namespace message {

    class Message {
    private:
        std::vector<std::byte> data_;

    public:
        Message() {}
        Message(std::vector<std::byte> data) {

        }

        std::vector<std::byte> data() const {
            return this->data_;
        }

        void data(std::byte *data, int size) {
            this->data_ = std::vector<std::byte>(data, data + size);
        }

        void data(std::vector<std::byte> data) {
            this->data_ = data;
        }
    };

    class HeartBeatMessage : Message {
    public:
        HeartBeatMessage() : Message() {}
    };

} // message


#endif //MULTIPATH_PROXY_MESSAGE_H
