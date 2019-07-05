#pragma once
#include <boost/asio.hpp>
#include <queue>
#include "particlesdeque.h"

using namespace boost::asio::ip;

class Client : public std::enable_shared_from_this<Client> {
public:

    Client(boost::asio::io_service& io_service, std::shared_ptr<ParticlesDeque> deq)
                : io_service{io_service}, deq{deq}, sock{io_service} { }

    tcp::socket& socket();

    void connect(const std::string host, const unsigned short port);

    void start(std::ifstream &data_file);

    void push_back(const std::vector<Particle>& p);

    void push_front(const std::vector<Particle>& p);

    void pop_back();

    void pop_front();

    std::vector<Particle> get_back();

    std::vector<Particle> get_front();

    void stop();

    void next();

private:
    volatile unsigned long long count = 0;
    boost::asio::io_service &io_service;
    tcp::socket sock;
    std::mutex mutex_msg;
    std::mutex mutex_deq;
    volatile bool isEmptyQueue = true;
    std::queue<std::string> msg_queue{};
    std::condition_variable cond_var;
    volatile bool isNeedClose = false;
    std::shared_ptr<ParticlesDeque> deq;
}; 
