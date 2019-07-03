#ifndef SCHOOL_SERVER_CLIENT_HPP
#define SCHOOL_SERVER_CLIENT_HPP

#include <boost/asio.hpp>
#include <queue>

using namespace boost::asio::ip;

class Client {
public:

    explicit Client(boost::asio::io_service& io_service) : io_service(io_service), sock{io_service} {}

    tcp::socket& socket();

    void connect(const std::string host, const unsigned short port);

    void start(std::ifstream &data_file);

    void stop();

    void next();

private:
    size_t count = 0;
    boost::asio::io_service &io_service;
    tcp::socket sock;
    std::mutex mutex;
    bool isEmptyQueue = true;
    std::queue<std::string> msg_queue{};
    std::condition_variable cond_var;
};

#endif //SCHOOL_SERVER_CLIENT_HPP
