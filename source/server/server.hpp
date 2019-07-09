#pragma once

#include <queue>
#include <set>
#include <mutex>
#include <condition_variable>
#include <boost/asio.hpp>
#include "../computer/ompComputer.h"
#include "serverParser.hpp"
#include "../computer/sequentialComputer.h"

using namespace boost::asio::ip;

class Server {

public:

    class Connection : public std::enable_shared_from_this<Connection> {
    public:
        Connection(boost::asio::io_service &io_service, Server &server) : sock(io_service), server(server) { }

        void write_message(const std::string&);

        void handle_read_command(const boost::system::error_code &er);

        void handle_read_count(const boost::system::error_code &er);

        void handle_read_data(const boost::system::error_code &er);

        void handle_write_message(const boost::system::error_code &er);

        inline tcp::socket& socket() { return sock; }

        inline ull get_count() const { return count; }

        void start();

    private:
        volatile ull count = 0;
        tcp::socket sock;
        char* read_msg = nullptr;
        std::string send_msg;
        Server &server;
        std::shared_ptr<Computer> computer;
        std::vector<Particle> data_particle{};
        std::mutex mutex{};
        std::deque<std::string> send_queue{};
    };

    Server(boost::asio::io_service &io_service, tcp::endpoint &endpoint, size_t count_nodes) : acceptor{io_service, endpoint},
                                                                            io_service{io_service}, count_nodes(count_nodes),
                                                                            nodes_weights{count_nodes}, computers{count_nodes} {
        std::thread check_thread{[this]() {
            timer.async_wait([this](const boost::system::error_code &errorCode) {
                this->check(errorCode);
            });
            timer_service.run();
        }};
        check_thread.detach();
    }

    Server(const Server&) = delete;

    Server& operator=(const Server&) = delete;

    Server(Server &&) = delete;

    Server&& operator=(Server &&) = delete;

    void start_working();

    void add_connection(const std::shared_ptr<Connection> &conn);

    void remove_connection(const std::shared_ptr<Server::Connection> &conn);

    std::shared_ptr<Computer> balance_weight(ull weight);

    ~Server() = default;

private:

    void handle(std::shared_ptr<Connection> connection, const boost::system::error_code &error_code);

    void check(const boost::system::error_code &);

    volatile int count_nodes;
    const ull MAX_WEIGHT = 5000;
    std::vector<std::shared_ptr<Computer>> computers;
    std::mutex conn_mutex{};
    std::set<std::shared_ptr<Connection>> connections{};
    boost::asio::io_service timer_service{};
    boost::asio::deadline_timer timer{timer_service, boost::posix_time::seconds{120}};
    boost::asio::io_service &io_service;
    tcp::acceptor acceptor;
    std::vector<ull> nodes_weights;
};
