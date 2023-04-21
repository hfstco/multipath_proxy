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

// dev exceptions

class NotImplementedError : public Exception {
public:
    NotImplementedError(const char *what_arg) : Exception(what_arg) {};

    NotImplementedError(std::string what_arg) : Exception(what_arg) {};
};

// net exceptions

class NetworkException : public Exception {
public:
    NetworkException(const char *what_arg) : Exception(what_arg) {};

    NetworkException(std::string what_arg) : Exception(what_arg) {};
};

class SocketException : public Exception {
public:
    SocketException(const char *what_arg) : Exception(what_arg) {};

    SocketException(std::string what_arg) : Exception(what_arg) {};
};

class SockAddrException : public Exception {
public:
    SockAddrException(const char *what_arg) : Exception(what_arg) {};

    SockAddrException(std::string what_arg) : Exception(what_arg) {};
};

// collections exception

class NotFoundException : public Exception {
public:
    NotFoundException(const char *what_arg) : Exception(what_arg) {};

    NotFoundException(std::string what_arg) : Exception(what_arg) {};
};

#endif //MULTIPATH_PROXY_EXCEPTION_H
