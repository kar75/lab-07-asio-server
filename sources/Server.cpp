// Copyright 2020

#include <boost/unordered_map.hpp>
#include <numeric>
#include <boost/thread/pthread/recursive_mutex.hpp>

#include "Server.hpp"
#include "Message.hpp"

std::unordered_map<std::string, std::string> getSocketInfo(const Server::Socket &socket)
{
    std::unordered_map<std::string, std::string> result;

    result["address"] = socket.remote_endpoint().address().to_string();
    result["port"] = std::to_string(socket.remote_endpoint().port());

    return result;
}

std::string getSocketInfoString(const Server::Socket &socket)
{
    auto info = getSocketInfo(socket);

    return info["address"] + " " + info["port"];
}

void Server::accept()
{
    Socket socket = acceptor_->accept();
    std::string info = getSocketInfoString(socket);

    auto client = std::make_shared<Client>(std::move(socket), ioContext_);
    Client::checkDeadline(client);

    BOOST_LOG_TRIVIAL(info) << "Connected: " << info;

    boost::recursive_mutex::scoped_lock lock{clientsMutex_};

    triggerClientsUpdate();
    clients_.push_back(client);
}

void Server::handleClients()
{
    boost::recursive_mutex::scoped_lock lock{clientsMutex_};

    for (std::shared_ptr<Client> client : clients_) {
        if (client->isClosed()) {
            continue;
        }

        auto &socket = client->getSocket();

        boost::asio::streambuf buffer{};

        try {
            client->readWithTimeout(buffer);
            std::string message{std::istreambuf_iterator<char>{&buffer},
                                std::istreambuf_iterator<char>{}};

            BOOST_LOG_TRIVIAL(debug) << "Message from (" << getSocketInfoString(socket) << "): '"
                                     << message << "'";

            std::string result = Message::passMessage(message, *this, *client);
            BOOST_LOG_TRIVIAL(debug) << "Message to (" << getSocketInfoString(socket) << "): '"
                                     << result << "'";

            client->writeWithTimeout(boost::asio::buffer(result, result.size()));
        } catch (MessageHandlerException &exception) {
            BOOST_LOG_TRIVIAL(error) << "Message exception to ('" << getSocketInfoString(socket)
                                     << "'): " << exception.what();
        } catch (std::runtime_error &exception) {
            BOOST_LOG_TRIVIAL(error) << "Exception: " << exception.what();
            BOOST_LOG_TRIVIAL(info) << "Is socket timed out: " << client->isTimedOut();
        }
    }

    // erase
    bool triggerUpdate = false;
    for (auto it = clients_.begin(); it != clients_.end();) {
        if ((*it)->isClosed()) {
            it = clients_.erase(it);
            triggerUpdate = true;
        } else {
            it++;
        }
    }
    if (triggerUpdate) {
        triggerClientsUpdate();
    }
}

bool Server::isNameAvailable(const std::string &name) const
{
    for (const auto &client : clients_) {
        if (client->getName() == name) {
            return false;
        }
    }

    return true;
}

std::string Server::getClientString() const
{
    std::string result;

    for (const auto &client : clients_) {
        if (std::string name = client->getName(); !name.empty()) {
            result += name + " ";
        }
    }

    return result;
}