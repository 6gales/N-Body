#include "server.hpp"
#include <mpi/mpi.h>
#include "../computer/abstractComputer.h"
#include <iostream>
#include "../computer/sequentialComputer.h"
#include "serverReader.hpp"
#include <memory>

using namespace boost::asio::ip;

std::vector<Particle> parse_message(const std::string &msg, std::shared_ptr<Computer> computer);

std::string convert_particles_to_msg(const std::vector<Particle> &particles);

std::string read_message(tcp::socket &sock) {
    boost::asio::streambuf buf;
    ServerReader ServerReader{buf};
    boost::asio::read(sock, buf, [&ServerReader](const boost::system::error_code &err_code, size_t bytes) -> size_t {
        return ServerReader.check_count_objects(err_code, bytes);
    });
    std::string msg{boost::asio::buffers_begin(buf.data()), boost::asio::buffers_end(buf.data())};
    return msg;
}

void write_message(tcp::socket &sock, const std::string &msg) {
    boost::asio::write(sock, boost::asio::buffer(msg));
}

void Server::start_accept() {
    std::shared_ptr<Connection> connection(new Connection(io_service));
    acceptor.async_accept(connection->socket(), [this, connection](const boost::system::error_code &error_code) {
        this->handle(connection, error_code);
    });
}

void Server::handle(std::shared_ptr<Connection> connection, const boost::system::error_code &error_code) {
    if (!error_code) {
        connection->start();
    }
    this->start_accept();
}

void Server::Connection::start() {
    auto ptr = this->shared_from_this();
    std::thread recv_thread([this, ptr] (){
        while (true) {
            const std::string msg = read_message(this->socket());
            auto result = parse_message(msg, computer);
            if (result.empty()) {
                //TODO break arrays
            } else {
                std::unique_lock<std::mutex> lck(mutex);
                particles_queue.push(result);
                isEmptyQueue = false;
                cond_var.notify_one();
            }

        }
    });
    recv_thread.detach();

    std::thread send_thread([this, ptr] () {
        while (true) {
            std::unique_lock<std::mutex> lck(mutex);
            while (isEmptyQueue) cond_var.wait(lck);
            const auto msg = convert_particles_to_msg(particles_queue.front());
            particles_queue.pop();
            if (particles_queue.empty()) isEmptyQueue = true;
            write_message(this->socket(), msg);
        }
    });
    send_thread.detach();
}

