// Copyright 2020

#include "Client.hpp"
#include <boost/log/trivial.hpp>

void setResult(boost::system::error_code *a, boost::system::error_code b)
{
    *a = b;
}

const int Client::TIMEOUT = 5;

void Client::readWithTimeout(StreamBuffer &buffer)
{
    if (isClosed()) {
        throw ClientConnectionAlreadyClosed{};
    }


    ErrorCode errorCode = boost::asio::error::would_block;
    // чтобы писалась ошибка, а не исключение

    ioContext_->restart();
    boost::asio::async_read_until(
            socket_,
            buffer,
            '\n',
            boost::bind(&setResult,
                        &errorCode,
                        boost::asio::placeholders::error)
    );

    do ioContext_->run_one(); while (errorCode == boost::asio::error::would_block);

    if (errorCode)
        throw boost::system::system_error(errorCode);
}

void Client::checkDeadline(std::weak_ptr<Client> clientPtr)
{
    auto client = clientPtr.lock();
    if (!client) {
        return;
    }

    if (client->deadline_.expires_at() <= DeadlineTimer::traits_type::now()) {
        client->deadline_.expires_at(boost::posix_time::pos_infin);
        client->isTimedOut_ = true;

        try {
            client->close();
            client->deadline_.cancel();
        } catch (const std::exception &) {
            // do nothing
        }

    }

    client->deadline_.async_wait(boost::bind(&Client::checkDeadline, clientPtr));
}

void Client::writeWithTimeout(const Buffer &buffer)
{
    if (isClosed()) {
        throw ClientConnectionAlreadyClosed{};
    }

    deadline_.expires_from_now(boost::posix_time::seconds{TIMEOUT});

    ErrorCode errorCode = boost::asio::error::would_block;

    ioContext_->restart();
    boost::asio::async_write(socket_, buffer, boost::bind(&setResult,
                                                          &errorCode,
                                                          boost::asio::placeholders::error));

    // Block until the asynchronous operation has completed.
    do ioContext_->run_one(); while (errorCode == boost::asio::error::would_block);

    if (errorCode)
        throw boost::system::system_error(errorCode);
}