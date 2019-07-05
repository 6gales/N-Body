#pragma once

#include<boost/asio.hpp>
#include <queue>
#include "../computer/ompComputer.h"
#include "serverParser.hpp"

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

        void add_msg(const std::string& msg);

        inline tcp::socket& socket() { return sock; }

        void start();

    private:
        boost::asio::io_service timer_service{};
        volatile bool isEmptyQueue = true;
        volatile bool isAlive = true;
        std::mutex mutex;
        std::condition_variable cond_var;
        tcp::socket sock;
        std::queue<std::string> particles_queue{};
        std::shared_ptr<Computer> computer{new ompComputer{4}};
        volatile CommandType commandType = CommandType::ALIVE_;
    };

    void handle(std::shared_ptr<Connection> connection, const boost::system::error_code &error_code);

    std::vector<std::shared_ptr<Connection>> connections;
    boost::asio::io_service &io_service;
    tcp::acceptor acceptor;
};
