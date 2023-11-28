// Copyright 2020

#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <chrono>
#include <thread>
#include <vector>
#include <boost/lexical_cast.hpp>

#include "Client.hpp"
#include "Server.hpp"

#include "header.hpp"


void accept_thread(Server &server)
{
    while (true) {
        server.accept();
    }
}

void handle_clients_thread(Server &server)
{
    using namespace std::chrono_literals;

    while (true) {
        std::this_thread::sleep_for(1ms);

        server.handleClients();
    }
}

int main(int argc, char *argv[])
{
    static const unsigned short DEFAULT_PORT = 8001;

    LogSetup::init();
    BOOST_LOG_TRIVIAL(debug) << "Log setup complete";

    unsigned short port = (argc >= 2)
                          ? boost::lexical_cast<unsigned short>(argv[1])
                          : DEFAULT_PORT;

    Server::Endpoint endpoint{boost::asio::ip::tcp::v4(), port};
    Server server{endpoint};

    BOOST_LOG_TRIVIAL(info) << "Server running on port " << port;

    boost::thread_group threads;
    threads.create_thread(
            boost::bind(accept_thread, std::ref(server))
    );
    BOOST_LOG_TRIVIAL(debug) << "Created accept thread";

    threads.create_thread(
            boost::bind(handle_clients_thread, std::ref(server))
    );
    BOOST_LOG_TRIVIAL(debug) << "Created handle thread";

    threads.join_all();
}