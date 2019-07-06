#include "server.hpp"
#include "serverReader.hpp"
#include "serverParser.hpp"
#include <mpi/mpi.h>
#include <thread>
#include <mutex>
#include "../computer/abstractComputer.h"
#include "../computer/sequentialComputer.h"

using namespace boost::asio::ip;

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

void Server::check(const boost::system::error_code &er) {
    conn_mutex.lock();
    for (auto it = this->connections.begin(); it != this->connections.end(); ++it) {
        if (it->unique() || !it->get()->alive()) {
            it->get()->socket().shutdown(tcp::socket::shutdown_both);
            it->get()->socket().close();
            connections.erase(it);
        } else  {
            it->get()->add_msg(std::string{"CHECK"});
            it->get()->set_alive(false);
        }
    }
    conn_mutex.unlock();

    timer.expires_at(timer.expires_at() + boost::posix_time::seconds(120));

    timer.async_wait([this](const boost::system::error_code &errorCode) {
        this->check(errorCode);
    });
}

void Server::start_working() {
    std::thread check_thread{[this]() {
        timer.async_wait([this](const boost::system::error_code &errorCode) {
            this->check(errorCode);
        });
        timer_service.run();
    }};
    std::shared_ptr<Connection> connection(new Connection(io_service));
    acceptor.async_accept(connection->socket(), [this, connection](const boost::system::error_code &error_code) {
        this->handle(connection, error_code);
    });
}

void Server::handle(std::shared_ptr<Connection> connection, const boost::system::error_code &error_code) {
    if (!error_code) {
        connection->start();
        conn_mutex.lock();
        connections.push_front(connection);
        conn_mutex.unlock();
    }
    this->start_working();
}

void Server::Connection::start() {

    auto ptr = this->shared_from_this();
    std::thread recv_thread([this, ptr] (){
        while (commandType != CommandType::STOP_) {
            std::string msg;
            try {
                msg = read_message(this->socket());
            } catch (const std::exception& ex) {
                std::cerr << ex.what() << "Close connection" << std::endl;
                commandType = CommandType::STOP_;
                std::unique_lock<std::mutex> lck(mutex);
                isEmptyQueue = false;
                cond_var.notify_one();
                continue;
            }
            std::vector<Particle> result;
            commandType = parse_message(msg, computer, result);
            if (commandType == CommandType::STOP_) {
                std::unique_lock<std::mutex> lck(mutex);
                isEmptyQueue = false;
                cond_var.notify_one();
            } else if (commandType == CommandType::START_ || commandType == CommandType::NEXT_) {
                std::unique_lock<std::mutex> lck(mutex);
                particles_queue.push(convert_particles_to_msg(result));
                isEmptyQueue = false;
                cond_var.notify_one();
            } else if (commandType == CommandType::ALIVE_) {
                this->set_alive(true);
            }
        }
    });
    recv_thread.detach();

    std::thread send_thread([this, ptr] () {
        while (commandType != CommandType::STOP_) {
            std::unique_lock<std::mutex> lck(mutex);
            while (isEmptyQueue) cond_var.wait(lck);
            if (commandType == CommandType::STOP_) break;
            const auto msg = particles_queue.front();
            particles_queue.pop();
            if (particles_queue.empty()) isEmptyQueue = true;
            write_message(this->socket(), msg);
        }
    });
    send_thread.detach();
}

void Server::Connection::add_msg(const std::string &msg) {
    particles_queue.push(msg);
}
