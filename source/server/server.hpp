#pragma once

#include<boost/asio.hpp>
#include <queue>
#include "../computer/ompComputer.h"

using namespace boost::asio::ip;

class Server {

public:

    Server(boost::asio::io_service &io_service, tcp::endpoint &endpoint) : acceptor(io_service, endpoint),
                                                                           io_service(io_service) {}
    void start_accept();

    ~Server() = default;

private:

    class Connection : public std::enable_shared_from_this<Connection> {
    public:
        explicit Connection(boost::asio::io_service &io_service) : sock(io_service) { }

        inline tcp::socket& socket() { return sock; }

        void start();

    private:
        volatile bool isEmptyQueue = true;
        std::mutex mutex;
        std::condition_variable cond_var;
        tcp::socket sock;
        std::queue<std::vector<Particle>> particles_queue{};
        std::shared_ptr<Computer> computer{new ompComputer{4}};
        volatile bool isNeedClose = false;
    };

    void handle(std::shared_ptr<Connection> connection, const boost::system::error_code &error_code);

    boost::asio::io_service &io_service;
    tcp::acceptor acceptor;
};
