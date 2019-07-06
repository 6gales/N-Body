#pragma once

#include<boost/asio.hpp>
#include <queue>
#include <mutex>
#include <condition_variable>
#include "../computer/ompComputer.h"
#include "serverParser.hpp"

using namespace boost::asio::ip;

class Server {

public:

    Server(boost::asio::io_service &io_service, tcp::endpoint &endpoint) : acceptor(io_service, endpoint),
                                                                           io_service(io_service) { }
    void start_working();

    ~Server() = default;

private:

    class Connection : public std::enable_shared_from_this<Connection> {
    public:
        explicit Connection(boost::asio::io_service &io_service) : sock(io_service) { }

        void add_msg(const std::string& msg);

        inline tcp::socket& socket() { return sock; }

        void start();

        inline bool alive() { return isAlive; }

        inline void set_alive(bool isAlive1) { this->isAlive = isAlive1; }

    private:
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

    void check(const boost::system::error_code &);

    std::deque<std::shared_ptr<Connection>> connections;
    std::mutex conn_mutex;
    boost::asio::io_service timer_service{};
    boost::asio::deadline_timer timer{timer_service, boost::posix_time::seconds{120}};
    boost::asio::io_service &io_service;
    tcp::acceptor acceptor;
};
