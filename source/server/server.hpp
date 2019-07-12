#pragma once

#include "../computer/computer.h"
#include <queue>
#include <set>
#include <mutex>
#include <condition_variable>
#include <boost/asio.hpp>

using namespace boost::asio::ip;

using ull = unsigned long long;

class Server {

public:

    class Connection : public std::enable_shared_from_this<Connection> {
    public:
        Connection(boost::asio::io_service &io_service, Server &server) : sock(io_service), server(server), key(Server::gen_new_key()) { }

        void write_message(const std::string&);

        void handle_read_command(const boost::system::error_code &er);

        void handle_read_count(const boost::system::error_code &er);

        void handle_read_data(const boost::system::error_code &er);

        void handle_write_message(const boost::system::error_code &er);

        inline tcp::socket& socket() { return sock; }

        const std::shared_ptr<Computer> &get_computer() const;

        void start();

        inline unsigned int get_key() const { return key; }

    private:
        volatile ull count = 0;
        tcp::socket sock;
        char* read_msg = nullptr;
        std::string send_msg;
        Server &server;
        std::shared_ptr<Computer> computer = nullptr;
        std::vector<Particle> data_particle{};
        std::mutex mutex{};
        std::deque<std::string> send_queue{};
        const unsigned int key;
    };

    Server(boost::asio::io_service &io_service, tcp::endpoint &endpoint, size_t count_nodes) : acceptor{io_service, endpoint},
                                                                            io_service{io_service}, count_nodes(count_nodes),
                                                                            computers{count_nodes} {
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

    ~Server() = default;

private:

    struct key_compare {
        bool operator()(const std::shared_ptr<Connection> &c1, const std::shared_ptr<Connection> &c2) const {
            return c1->get_key() < c2->get_key();
        }
    };

    std::shared_ptr<Computer> search_computer(unsigned int key);

    std::shared_ptr<Computer> create_computer(ull weight);

    void remove_connection(const std::shared_ptr<Server::Connection> &conn);

    void add_connection(const std::shared_ptr<Connection> &conn);

    static void add_new_free_key(unsigned int key);

    static unsigned int gen_new_key();

    void handle(std::shared_ptr<Connection> connection, const boost::system::error_code &error_code);

    void check(const boost::system::error_code &);

    static std::mutex keys_mutex;
    static unsigned int keys;
    static std::deque<unsigned int> free_keys;

    volatile bool isExistSeqComp = false;
    volatile int count_nodes;
    ull MAX_WEIGHT = 10000;
    std::mutex comp_mutex{};
    std::vector<std::shared_ptr<Computer>> computers;
    std::mutex conn_mutex{};
    std::set<std::shared_ptr<Connection>, key_compare> connections{};
    boost::asio::io_service timer_service{};
    boost::asio::deadline_timer timer{timer_service, boost::posix_time::seconds{120}};
    boost::asio::io_service &io_service;
    tcp::acceptor acceptor;
};
