#include "IOException.hpp"
#include "client.hpp"
#include "clientParser.hpp"
#include <boost/bind.hpp>
#include <thread>
#include <mutex>

using namespace boost::asio::ip;

using ull = unsigned long long;


Client::Client(boost::asio::io_service &io_service)
            : io_service{io_service}, sock{io_service} { }

void Client::connect(basic_resolver_results<tcp> ep, std::ifstream &data_file) {
    COUNT = parse_file(data_file, start_msg, particles_mass, first_particles);
    if (COUNT == 0) {
        return;
    }
    boost::asio::async_connect(sock, ep,
                               boost::bind(&Client::handle_connect, this, boost::asio::placeholders::error));
    std::thread t{boost::bind(&boost::asio::io_service::run, &io_service)};
    t.detach();
}

void Client::write_msg(const std::string msg) {
    send_queue.push_back(msg);
    boost::asio::async_write(sock, boost::asio::buffer(send_queue.front().c_str(), send_queue.front().length()),
                boost::bind(&Client::handle_write_msg, this, boost::asio::placeholders::error));
}

void Client::handle_connect(const boost::system::error_code &er) {
    if (!er) {
        read_msg = new char[sizeof(float)*4*5000];
        boost::asio::async_read(sock, boost::asio::buffer(read_msg, 5),
                                boost::bind(&Client::handle_read_command, this, boost::asio::placeholders::error));
    } else {
//        std::cerr << "ERROR" << std::endl;
    }
}

void Client::handle_read_command(const boost::system::error_code &er) {
    if (!er) {
//        std::cerr << std::string{read_msg} << std::endl;
        if (!strncmp(read_msg, "DATA ", 5)) {
            boost::asio::async_read(sock, boost::asio::buffer(read_msg, 8),
                  boost::bind(&Client::handle_read_count, this, boost::asio::placeholders::error));
        } else if (!strncmp(read_msg, "CHECK", 5)) {
            write_msg(std::string{"ALIVE"});
            boost::asio::async_read(sock, boost::asio::buffer(read_msg, 5),
                        boost::bind(&Client::handle_read_command, this, boost::asio::placeholders::error));
        }
    } else {
//        std::cerr << "ERROR" << std::endl;
    }
}

void Client::handle_read_count(const boost::system::error_code &er) {
    if (!er) {
        std::string str_msg{read_msg, 8};
        part_count = (((ull)str_msg.at(0) << 56) & 0xFF00000000000000) | (((ull)str_msg.at(1) << 48) & 0x00FF000000000000) | (((ull)str_msg.at(2) << 40) & 0x0000FF0000000000)
                                     | (((ull)str_msg.at(3) << 32) & 0x000000FF00000000) | (((ull)str_msg.at(4) << 24) & 0x00000000FF000000) | (((ull)str_msg.at(5) << 16) & 0x0000000000FF0000)
                                     | (((ull)str_msg.at(6) << 8) & 0x000000000000FF00) | ((ull)str_msg.at(7) & 0x00000000000000FF);
        if (part_count == (COUNT - (((COUNT/5000+1)/2)*5000))) next();
        if (part_count >= 5000) {
            part_count -= 5000;
            boost::asio::async_read(sock, boost::asio::buffer(read_msg, sizeof(float)*4*5000),
                  boost::bind(&Client::handle_buffered_read, this, boost::asio::placeholders::error));
        } else {
            boost::asio::async_read(sock, boost::asio::buffer(read_msg, sizeof(float)*4*part_count),
                  boost::bind(&Client::handle_read_data, this, boost::asio::placeholders::error));
        }
    } else {

    }
}

void Client::handle_buffered_read(const boost::system::error_code &er) {
    if (!er) {
        auto particles = parse_data_msg(read_msg, 5000);
        push_back_dyn(particles);
        push_back_map(particles);
        if (part_count == (COUNT - (((COUNT/5000+1)/2)*5000))) next();
        if (part_count >= 5000) {
            part_count -= 5000;
            boost::asio::async_read(sock, boost::asio::buffer(read_msg, sizeof(float)*4*5000),
                  boost::bind(&Client::handle_buffered_read, this, boost::asio::placeholders::error));
        } else {
            boost::asio::async_read(sock, boost::asio::buffer(read_msg, sizeof(float)*4*part_count),
                  boost::bind(&Client::handle_read_data, this, boost::asio::placeholders::error));
        }
    } else {

    }
}

void Client::handle_read_data(const boost::system::error_code &er) {
    if (!er) {
//        std::cerr << std::string(read_msg).size() << std::endl;
        auto particles = parse_data_msg(read_msg, part_count);
        push_back_dyn(particles);
        push_back_map(particles);
        boost::asio::async_read(sock, boost::asio::buffer(read_msg, 5),
              boost::bind(&Client::handle_read_command, this, boost::asio::placeholders::error));
    } else {
//        std::cerr << "ERROR" << std::endl;
    }
}

void Client::handle_write_msg(const boost::system::error_code &er) {
    if (!er) {
        send_queue.pop_front();
        if (!send_queue.empty()) {
            boost::asio::async_write(sock, boost::asio::buffer(send_queue.front().c_str(), send_queue.front().length()),
                        boost::bind(&Client::handle_write_msg, this, boost::asio::placeholders::error));
        }
    } else {
//        std::cerr << "ERROR" << std::endl;
    }
}

tcp::socket& Client::socket() {
    return sock;
}

void Client::close() {
    sock.close();
}

void Client::setIsMap(bool isMap) {
    this->isMap = isMap;
}

void Client::setIsPaused(bool isPaused) {
    this->isPaused = isPaused;
}

void Client::push_back_dyn(const std::vector<Particle>& p) {
    mutex_deq_dyn.lock();
    deq_dynamic.push_back(p);
    mutex_deq_dyn.unlock();
}

void Client::push_front_dyn(const std::vector<Particle>& p) {
    mutex_deq_dyn.lock();
    deq_dynamic.push_front(p);
    mutex_deq_dyn.unlock();
}

void Client::pop_back_dyn() {
    mutex_deq_dyn.lock();
    deq_dynamic.pop_back();
    mutex_deq_dyn.unlock();
}

void Client::pop_front_dyn() {
    mutex_deq_dyn.lock();
    deq_dynamic.pop_front();
    mutex_deq_dyn.unlock();
}

std::vector<Particle> Client::get_back_dyn() {
    mutex_deq_dyn.lock();
    auto res = deq_dynamic.get_back();
    mutex_deq_dyn.unlock();
    return res;
}

std::vector<Particle> Client::get_front_dyn() {
    mutex_deq_dyn.lock();
    auto res = deq_dynamic.get_front();
    mutex_deq_dyn.unlock();
    return res;
}

void Client::push_back_map(const std::vector<Particle>& p) {
    if (!isMap) return;
    mutex_deq_map.lock();
    deq_map.push_back(p);
    mutex_deq_map.unlock();
}

void Client::push_front_map(const std::vector<Particle>& p) {
    if (!isMap) return;
    mutex_deq_map.lock();
    deq_map.push_front(p);
    mutex_deq_map.unlock();
}

void Client::pop_back_map() {
    mutex_deq_map.lock();
    deq_map.pop_back();
    mutex_deq_map.unlock();
}

void Client::pop_front_map() {
    mutex_deq_map.lock();
    deq_map.pop_front();
    mutex_deq_map.unlock();
}

std::vector<Particle> Client::get_back_map() {
    mutex_deq_map.lock();
    auto res = deq_map.get_back();
    mutex_deq_map.unlock();
    return res;
}

std::vector<Particle> Client::get_front_map() {
    mutex_deq_map.lock();
    auto res = deq_map.get_front();
    mutex_deq_map.unlock();
    return res;
}

void Client::delete_deque_map() {
    deq_map.delete_deque();
}

const std::vector<float> &Client::get_particles_mass() const {
    return particles_mass;
}

const std::vector<Particle> &Client::get_first_particles() const {
    return first_particles;
}

ull Client::get_count() const {
    return COUNT;
}

void Client::resume() {
    isPaused = false;
    next();
}

void Client::start() {
    if (!isStart) {
        write_msg(start_msg);
        isStart = true;
    }
}

void Client::stop() {
    write_msg(std::string{"STOP "});
    close();
}

void Client::pause() {
    write_msg(std::string{"PAUSE"});
    isPaused = true;

}

void Client::next() {
    if (isPaused) return;
    write_msg(std::string{"NEXT "});
}
