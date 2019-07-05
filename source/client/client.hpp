#pragma once
#include <boost/asio.hpp>
#include <queue>

using namespace boost::asio::ip;

class Client : public std::enable_shared_from_this<Client> {
public:

    explicit Client(boost::asio::io_service& io_service) : io_service(io_service), sock{io_service} {}

    tcp::socket& socket();

    void connect(const std::string host, const unsigned short port);

    void start(std::ifstream &data_file);

    void stop();

    void next();

private:
    volatile unsigned long long count = 0;
    boost::asio::io_service &io_service;
    tcp::socket sock;
    std::mutex mutex;
    volatile bool isEmptyQueue = true;
    std::queue<std::string> msg_queue{};
    std::condition_variable cond_var;
    volatile bool isNeedClose = false;

};
