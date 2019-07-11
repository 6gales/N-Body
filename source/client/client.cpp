#include "IOException.hpp"
#include "client.hpp"
#include "clientParser.hpp"
#include <boost/bind.hpp>
#include <thread>
#include <mutex>

using namespace boost::asio::ip;

using ull = unsigned long long;


Client::Client(std::string host, unsigned short port, std::ifstream& data_file)
            : sock{io_service} {
    tcp::resolver resolver(io_service);

    auto ep = resolver.resolve(host, std::to_string(port));

    std::string start_msg;
    COUNT = parse_file(data_file, start_msg, particles_mass);
    if (COUNT == 0) {
        //TODO kill client
    }
    boost::asio::async_connect(sock, ep,
                               boost::bind(&Client::handle_connect, this, boost::asio::placeholders::error));
    std::thread t{boost::bind(&boost::asio::io_service::run, &io_service)};
    t.detach();

    write_msg(start_msg);

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
        push_back(particles);
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
        push_back(particles);
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

void Client::push_back(const std::vector<Particle>& p) {
	mutex_deq.lock();
    deq.push_back(p);
	mutex_deq.unlock();
}

void Client::push_front(const std::vector<Particle>& p) {
	mutex_deq.lock();
    deq.push_front(p);
	mutex_deq.unlock();
}

void Client::pop_back() {
	mutex_deq.lock();
    deq.pop_back();
	mutex_deq.unlock();
}

void Client::pop_front() {
	mutex_deq.lock();
    deq.pop_front();
	mutex_deq.unlock();
}

std::vector<Particle> Client::get_back() {
	mutex_deq.lock();
    auto res = deq.get_back();
	mutex_deq.unlock();
    return res;
}

std::vector<Particle> Client::get_front() {
	mutex_deq.lock();
    auto res = deq.get_front();
	mutex_deq.unlock();
    return res;
}

const std::vector<float> &Client::get_particles_mass() const {
    return particles_mass;
}

ull Client::get_count() const {
    return COUNT;
}

void Client::stop() {
    write_msg(std::string{"STOP "});
}

void Client::next() {
    write_msg(std::string{"NEXT "});
}

void Client::pause() {
    write_msg(std::string{"PAUSE"});
}
