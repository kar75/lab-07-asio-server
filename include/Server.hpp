// Copyright 2020

#ifndef INCLUDE_SERVER_HPP
#define INCLUDE_SERVER_HPP

#include <boost/asio.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/log/trivial.hpp>
#include "Client.hpp"

class Server
{
public:
    using Endpoint = boost::asio::ip::tcp::endpoint;
    using Acceptor = boost::asio::ip::tcp::acceptor;
    using Context = boost::asio::io_context;
    using Socket = boost::asio::ip::tcp::socket;

    explicit Server(const Endpoint &endpoint)
            : endpoint_(endpoint),
              acceptor_(std::make_unique<Acceptor>(*ioContext_, endpoint))
    {}

    void accept();

    void handleClients();

    [[nodiscard]] bool isNameAvailable(const std::string &name) const;

    [[nodiscard]] const std::vector<std::shared_ptr<Client>> &getClients() const
    {
        return clients_;
    }

    [[nodiscard]] std::string getClientString() const;

private:
    void triggerClientsUpdate() {
        for (auto &clientFromList : clients_) {
            clientFromList->setUpdateList(true);
        }
    }

    std::shared_ptr<Context> ioContext_ = std::make_shared<Context>();
    Endpoint endpoint_;
    std::unique_ptr<Acceptor> acceptor_;

    boost::recursive_mutex clientsMutex_;
    std::vector<std::shared_ptr<Client>> clients_{};
};

#endif //INCLUDE_SERVER_HPP
