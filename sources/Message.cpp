// Copyright 2020

#include "Message.hpp"
#include "Server.hpp"

#define HANDLER_FUNCTION(functionName, argsName, serverName, clientName) \
std::string functionName(const std::string &argsName, Server &serverName, Client &clientName)

std::string Message::passMessage(const std::string &message, Server &server, Client &client)
{
    for (auto &pair: handlers) {
        boost::regex regex{pair.first};
        boost::smatch result;

        if (!boost::regex_match(message, result, regex)) {
            continue;
        }

        return pair.second(result[2], server, client);
    }

    throw NoMessageFound{message};
}

HANDLER_FUNCTION(login,name,server,client)
{
    if (!server.isNameAvailable(name)) {
        throw MessageHandlerException{"The name '" + name +"' is not available"};
    }

    client.setName(name);

    return "login ok\n";
}

HANDLER_FUNCTION(clientList,,server,client)
{
    client.setUpdateList(false);

    return server.getClientString() + "\n";
}

HANDLER_FUNCTION(ping,,,client)
{
    if (client.isUpdateList()) {
        return "client_list_changed\n";
    }

    return "ping ok\n";
}

const Message::HandlersType Message::handlers = {
        {"(login) +([a-zA-Z0-9_\\-]{1,32}) *.*", login},
        {"clients *.*" + std::string{"\n"}, clientList},
        {"ping *.*" + std::string{"\n"}, ping},
};