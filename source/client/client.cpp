#include "clientReader.hpp"
#include "IOException.hpp"
#include "client.hpp"
#include "../computer/abstractComputer.h"
#include <iostream>
#include <fstream>
#include <queue>

using namespace boost::asio::ip;

size_t parse_file(std::istream &data_file, std::string &start_message);

std::vector<Particle> parse_msg_from_server(const std::string &msg);

void printParticles(const std::vector<Particle> &particles);

std::string read_message(tcp::socket &sock) {
    boost::asio::streambuf buf;
    ClientReader reader{buf};
    boost::asio::read(sock, buf, [&reader](const boost::system::error_code& err_code, size_t bytes) -> size_t {
        reader.check_count_objects(err_code, bytes);
    });
    std::string msg = std::string{boost::asio::buffers_begin(buf.data()), boost::asio::buffers_end(buf.data())};

    return msg;
}

void write_message(tcp::socket &sock, const std::string &msg) {
    boost::asio::write(sock, boost::asio::buffer(msg));
}

tcp::socket& Client::socket() {
    return sock;
}

void Client::connect(const std::string host, const unsigned short port) {
    tcp::endpoint ep{address::from_string(host), port};
    sock.connect(ep);
    auto ptr = this->shared_from_this();

    std::thread write_thread{[ptr, this]() {
        while (!isNeedClose) {
            std::unique_lock<std::mutex> lck(mutex);
            while (isEmptyQueue) cond_var.wait(lck);
            if (isNeedClose && msg_queue.empty()) break;
            const auto msg = msg_queue.front();
            msg_queue.pop();
            if (msg_queue.empty()) isEmptyQueue = true;
            write_message(this->socket(), msg);
        }
    }};
    write_thread.detach();

    std::thread read_thread{[ptr, this]() {
        while (!isNeedClose) {
            std::string msg;
            try {
                msg = read_message(this->socket());
            } catch (std::exception &ex) {
                std::cerr << "Error while read from socket: " << ex.what() << std::endl;
                isNeedClose = true;
                std::unique_lock<std::mutex> lck(mutex);
                isEmptyQueue = false;
                cond_var.notify_one();
                continue;
            }
            auto particles = parse_msg_from_server(msg);
            printParticles(particles);
            //TODO convert msg to good format for visualisator
            //TODO maybe call method 'next'
        }
    }};
    read_thread.detach();
}

void Client::start(std::ifstream &data_file) {
    std::string start_message{};
    count = parse_file(data_file, start_message);

    std::unique_lock<std::mutex> lck(mutex);
    msg_queue.push(start_message);
    isEmptyQueue = false;
    cond_var.notify_one();

}

void Client::stop() {
    std::unique_lock<std::mutex> lck(mutex);
    msg_queue.push(std::string{"STOP "});
    isEmptyQueue = false;
    cond_var.notify_one();
    isNeedClose = true;
}

void Client::next() {
    std::unique_lock<std::mutex> lck(mutex);
    msg_queue.push(std::string{"NEXT "});
    isEmptyQueue = false;
    cond_var.notify_one();
}