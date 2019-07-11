#pragma once
#include <mutex>
#include <condition_variable>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <queue>
#include "particlesdeque.h"

using namespace boost::asio::ip;

class Client {
public:

    Client(std::string host, unsigned short port, std::ifstream& data_file);

    tcp::socket& socket();

    void connect(basic_resolver_results<tcp> ep);

    void write_msg(const std::string msg);

    void handle_read_command(const boost::system::error_code &er);

    void handle_read_count(const boost::system::error_code &er);

    void handle_buffered_read(const boost::system::error_code &er);

    void handle_read_data(const boost::system::error_code &er);

    void handle_write_msg(const boost::system::error_code &er);

    void handle_connect(const boost::system::error_code &er);

    void start(std::ifstream &data_file, basic_resolver_results<tcp> ep);

    void push_back(const std::vector<Particle>& p);

    void push_front(const std::vector<Particle>& p);

    void pop_back();

    void pop_front();

    std::vector<Particle> get_back();

    std::vector<Particle> get_front();

    unsigned long long get_count() const;

    const std::vector<float>& get_particles_mass() const;

    void stop();

    void next();

    void pause();

    void close();

    ~Client() noexcept {
        delete[] read_msg;
    }

private:
    mutable unsigned long long COUNT;
    volatile unsigned long long part_count = 0;
    char* read_msg;
    ParticlesDeque deq{};
    boost::asio::io_service io_service{};
    tcp::socket sock;
    std::vector<float> particles_mass;
    std::mutex mutex_deq;
    std::deque<std::string> send_queue{};
};
