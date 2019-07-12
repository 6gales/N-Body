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

    Client(boost::asio::io_service &io_service);

    tcp::socket& socket();

    void connect(basic_resolver_results<tcp> ep, std::ifstream &data_file);

    void write_msg(const std::string msg);

    void handle_read_command(const boost::system::error_code &er);

    void handle_read_count(const boost::system::error_code &er);

    void handle_buffered_read(const boost::system::error_code &er);

    void handle_read_data(const boost::system::error_code &er);

    void handle_write_msg(const boost::system::error_code &er);

    void handle_connect(const boost::system::error_code &er);

    void setIsMap(bool isMap);

    void setIsPaused(bool isPaused);

    void push_back_dyn(const std::vector<Particle>& p);

    void push_front_dyn(const std::vector<Particle>& p);

    void pop_back_dyn();

    void pop_front_dyn();

    std::vector<Particle> get_back_dyn();

    std::vector<Particle> get_front_dyn();

    void push_back_map(const std::vector<Particle>& p);

    void push_front_map(const std::vector<Particle>& p);

    void pop_back_map();

    void pop_front_map();

    std::vector<Particle> get_back_map();

    std::vector<Particle> get_front_map();

    void delete_deque_map();

    unsigned long long get_count() const;

    const std::vector<Particle>& get_first_particles() const;

    const std::vector<float>& get_particles_mass() const;

    void start();

    void stop();

    void resume();

    void pause();

    void close();

    ~Client() noexcept {
        delete[] read_msg;
    }

private:
    void next();

    std::string start_msg;
    volatile bool isStart = false;
    volatile bool isPaused = false;
    volatile bool isMap = false;
    mutable unsigned long long COUNT;
    volatile unsigned long long part_count = 0;
    char* read_msg;
    ParticlesDeque deq_dynamic{}, deq_map{};
    boost::asio::io_service &io_service;
    tcp::socket sock;
    std::vector<Particle> first_particles;
    std::vector<float> particles_mass;
    std::mutex mutex_deq_dyn;
    std::mutex mutex_deq_map;
    std::deque<std::string> send_queue{};
};
