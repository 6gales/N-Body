#include "server.hpp"
#include "serverParser.hpp"
#include <boost/bind.hpp>
#include <thread>
#include <mutex>

using namespace boost::asio::ip;

std::mutex Server::keys_mutex{};
unsigned int Server::keys = 0;
std::deque<unsigned int> Server::free_keys{};

void Server::Connection::write_message(const std::string &msg) {
    mutex.lock();
    send_queue.push_back(msg);
    send_msg = send_queue.front();
    send_queue.pop_front();
    boost::asio::async_write(sock, boost::asio::buffer(send_msg.c_str(), send_msg.length()),
            boost::bind(&Connection::handle_write_message, shared_from_this(), boost::asio::placeholders::error));
    mutex.unlock();
}

void Server::Connection::handle_read_command(const boost::system::error_code &er) {
    if (!er) {
        if (!strncmp(read_msg, "START", 5)) {
            delete[] read_msg;
            read_msg = new char[8];
            boost::asio::async_read(sock, boost::asio::buffer(read_msg, 8),
                    boost::bind(&Connection::handle_read_count, shared_from_this(), boost::asio::placeholders::error));
        } else {
            if (!strncmp(read_msg, "NEXT ", 5)) {
                if (computer == nullptr) {
                    server.remove_connection(shared_from_this());
                    delete read_msg;
                    return;
                }
                data_particle = computer.get()->iterate();
                if (data_particle.empty()) {
                    computer = server.search_computer(key);
                    data_particle = computer.get()->iterate();
                }
                std::string msg = convert_particles_to_msg(data_particle, count);
                write_message(msg);
            } else if (!strncmp(read_msg, "STOP ", 5)) {
                server.remove_connection(shared_from_this());
                delete read_msg;
                return;
            } else if (!strncmp(read_msg, "PAUSE", 5)) {
                //wait command "next"
            }
            boost::asio::async_read(sock, boost::asio::buffer(read_msg, 5),
                    boost::bind(&Connection::handle_read_command, shared_from_this(), boost::asio::placeholders::error));
        }
    } else {
        server.remove_connection(shared_from_this());
    }
}

void Server::Connection::handle_read_count(const boost::system::error_code &er) {
    if (!er) {
        computer = server.create_computer(count);
        std::string str_msg{read_msg, 8};
        count = (((ull)str_msg.at(0) << 56) & 0xFF00000000000000) | (((ull)str_msg.at(1) << 48) & 0x00FF000000000000) | (((ull)str_msg.at(2) << 40) & 0x0000FF0000000000)
                              | (((ull)str_msg.at(3) << 32) & 0x000000FF00000000) | (((ull)str_msg.at(4) << 24) & 0x00000000FF000000) | (((ull)str_msg.at(5) << 16) & 0x0000000000FF0000)
                              | (((ull)str_msg.at(6) << 8) & 0x000000000000FF00) | ((ull)str_msg.at(7) & 0x00000000000000FF);
        delete[] read_msg;
        read_msg = new char[sizeof(float)*7*count];
        boost::asio::async_read(sock, boost::asio::buffer(read_msg, sizeof(float)*7*count),
                boost::bind(&Connection::handle_read_data, shared_from_this(), boost::asio::placeholders::error));
    } else {
        server.remove_connection(shared_from_this());
    }
}

void Server::Connection::handle_read_data(const boost::system::error_code &er) {
    if (!er) {
        data_particle = parse_start_message(read_msg, count);
        computer.get()->init(data_particle, count);
        data_particle = computer.get()->iterate();
        if (data_particle.empty()) {
            computer = server.search_computer(key);
            data_particle = computer.get()->iterate();
        }
        std::string msg = convert_particles_to_msg(data_particle, count);
        write_message(msg);
        delete[] read_msg;
        read_msg = new char[5];
        boost::asio::async_read(sock, boost::asio::buffer(read_msg, 5),
                boost::bind(&Connection::handle_read_command, shared_from_this(), boost::asio::placeholders::error));
    } else {
        server.remove_connection(shared_from_this());
    }
}

void Server::Connection::handle_write_message(const boost::system::error_code &er) {
    if (!er) {
        mutex.lock();
        if (!send_queue.empty()) {
            send_msg = send_queue.front();
            send_queue.pop_front();
            boost::asio::async_write(sock, boost::asio::buffer(send_msg.c_str(), send_msg.length()),
                                     boost::bind(&Connection::handle_write_message, shared_from_this(), boost::asio::placeholders::error));
        }
        mutex.unlock();
    } else {
        server.remove_connection(shared_from_this());
    }
}

void Server::Connection::start() {
    server.add_connection(shared_from_this());
    read_msg = new char[5];
    boost::asio::async_read(sock, boost::asio::buffer(read_msg, 5),
                            boost::bind(&Connection::handle_read_command, shared_from_this(),
                                        boost::asio::placeholders::error));
}

void Server::check(const boost::system::error_code &er) {
    if (!er) {
        conn_mutex.lock();
        for (const auto &connection : this->connections) {
            std::string check_msg{"CHECK"};
            connection.get()->write_message(check_msg);
        }
        conn_mutex.unlock();

        timer.expires_at(timer.expires_at() + boost::posix_time::seconds(120));

        timer.async_wait(boost::bind(&Server::check, this, boost::asio::placeholders::error));
    } else {
        std::cerr << "CRITICAL ERROR: " << er.message() << std::endl;
    }
}

void Server::start_working() {
    std::shared_ptr<Connection> connection(new Connection(io_service, *this));
    acceptor.async_accept(connection->socket(), boost::bind(&Server::handle, this, connection, boost::asio::placeholders::error));
}

void Server::handle(std::shared_ptr<Connection> connection, const boost::system::error_code &error_code) {
    if (!error_code) {
        connection->start();
        this->start_working();
    } else {
        std::cerr << "client couldn't connect, reason: " << error_code.message() << std::endl;
    }
}

void Server::add_connection(const std::shared_ptr<Server::Connection> &conn) {
    conn_mutex.lock();
    connections.insert(conn);
    conn_mutex.unlock();
}

void Server::remove_connection(const std::shared_ptr<Server::Connection> &conn) {
    conn_mutex.lock();
    if (connections.find(conn) == connections.end()) {
        conn_mutex.unlock();
        return;
    }
    unsigned int key = connections.find(conn)->get()->get_key();
    connections.erase(conn);
    conn_mutex.unlock();
    add_new_free_key(key);
}

std::shared_ptr<Computer> Server::create_computer(ull weight) {
    comp_mutex.lock();
    //TODO create balance
    for (auto & computer : computers) {
        if (computer == nullptr) {
            computer = std::shared_ptr<Computer>(new ompComputer(4));
            comp_mutex.unlock();
            return computer;
        } else if (computer->getSize() > MAX_WEIGHT) {
            continue;
        }
    }
    comp_mutex.unlock();
    MAX_WEIGHT += MAX_WEIGHT;
    return computers[0];
//    return create_computer(weight);
}

void Server::add_new_free_key(unsigned int key) {
    keys_mutex.lock();
    free_keys.push_back(key);
    keys_mutex.unlock();
}

unsigned int Server::gen_new_key() {
    keys_mutex.lock();
    if (!free_keys.empty()) {
        unsigned int key = free_keys.front();
        free_keys.pop_front();
        keys_mutex.unlock();
        return key;
    }
    keys_mutex.unlock();
    return ++keys;
}

std::shared_ptr<Computer> Server::search_computer(unsigned int key) {
    comp_mutex.lock();
    for (auto &comp : computers) {
        //TODO search by key
    }
    comp_mutex.unlock();
}
