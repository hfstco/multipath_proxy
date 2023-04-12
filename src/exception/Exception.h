//
// Created by Matthias Hofstätter on 23.02.23.
//

#ifndef MULTIPATH_PROXY_EXCEPTION_H
#define MULTIPATH_PROXY_EXCEPTION_H

#include <exception>
#include <string>


class Exception : std::exception {
public:
    Exception(const char *what_arg) : _message(what_arg) {};

    Exception(std::string what_arg) : _message(what_arg) {};

    const char *what() { return _message.c_str(); }

private:
    const std::string _message;
};

class NotImplementedError : public Exception {
public:
    NotImplementedError(const char *what_arg) : Exception(what_arg) {};

    NotImplementedError(std::string what_arg) : Exception(what_arg) {};
};

class NetworkError : public Exception {
public:
    NetworkError(const char *what_arg) : Exception(what_arg) {};

    NetworkError(std::string what_arg) : Exception(what_arg) {};
};

class SocketError : public Exception {
public:
    SocketError(const char *what_arg) : Exception(what_arg) {};

    SocketError(std::string what_arg) : Exception(what_arg) {};
};

class SockAddrError : public Exception {
public:
    SockAddrError(const char *what_arg) : Exception(what_arg) {};

    SockAddrError(std::string what_arg) : Exception(what_arg) {};
};


#endif //MULTIPATH_PROXY_EXCEPTION_H
