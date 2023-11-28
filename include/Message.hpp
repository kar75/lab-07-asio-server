// Copyright 2020

#ifndef INCLUDE_MESSAGE_HPP
#define INCLUDE_MESSAGE_HPP

#include <boost/unordered_map.hpp>
#include <boost/regex.hpp>
#include "Client.hpp"

class Server;

class MessageHandler
{
public:
    using HandlerFunction = std::string (*)(const std::string &, Server &, Client &);
    using HandlersType = boost::unordered_map<std::string, HandlerFunction>;

    static std::string passMessage(const std::string& message, Server &server, Client &client);

private:
    static const HandlersType handlers;
};

class MessageHandlerException: public std::runtime_error
{
public:
    using std::runtime_error::runtime_error;
};

class NoMessageFound: public MessageHandlerException {
public:
    explicit NoMessageFound(const std::string &message)
            : MessageHandlerException("No valid regex for message '" + message + "'") {}
};

#endif //INCLUDE_MESSAGE_HPP
