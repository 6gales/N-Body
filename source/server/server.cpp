#include <boost/asio.hpp>
#include <mpi/mpi.h>
#include "../computer/abstractComputer.h"
#include <queue>
#include <iostream>
#include "../computer/sequentialComputer.h"

using namespace boost::asio::ip;

const Particle* parse_message(const std::string &msg, std::shared_ptr<Computer> computer);

std::string construct_data_message(const Particle* particles, size_t count);

std::string read_message(tcp::socket &sock) {
    boost::asio::streambuf buf;
    boost::asio::read_until(sock, buf, "\r\n");
    std::string msg{boost::asio::buffers_begin(buf.data()), boost::asio::buffers_end(buf.data())};
    return msg;
}

void write_message(tcp::socket &sock, const std::string &msg) {
    boost::asio::write(sock, boost::asio::buffer(msg));
}

class Server {

public:

    Server(boost::asio::io_service &io_service, tcp::endpoint &endpoint) : acceptor(io_service, endpoint),
                                                                           io_service(io_service) {}

    void start_accept();

    ~Server() = default;

private:

    class Connection {
    public:
        explicit Connection(boost::asio::io_service &io_service) : sock(io_service) { }

        tcp::socket& socket() {
            return sock;
        }

        void start() {
            std::thread recv_thread([this] (){
                while (true) {
                    const std::string msg = read_message(sock);
                    const Particle* result = parse_message(msg, computer);
                    if (result == nullptr) {
                        //TODO break arrays
                    } else {
                        particles_queue.push(result);
                        isEmptyQueue = false;
                        cond_var.notify_one();
                    }
                        
                }
            });
            recv_thread.detach();

            std::thread send_thread([this] () {
                while (true) {
                    std::unique_lock<std::mutex> lck(mutex);
                    while (isEmptyQueue) cond_var.wait(lck);
                    mutex.lock();
                    const auto msg = construct_data_message(particles_queue.front(), computer->getSize());
                    particles_queue.pop();
                    if (particles_queue.empty()) isEmptyQueue = true;
                    mutex.unlock();
                    write_message(sock, msg);
                }
            });
            send_thread.detach();
        }

    private:
        bool isEmptyQueue = true;
        std::mutex mutex;
        std::condition_variable cond_var;
        tcp::socket sock;
        std::queue<const Particle*> particles_queue{};
        std::shared_ptr<Computer> computer;
    };

    void handle(const boost::shared_ptr<Connection> &connection, const boost::system::error_code &error_code) {
        if (!error_code) {
            connection->start();
        }
        this->start_accept();
    }

    boost::asio::io_service &io_service;
    tcp::acceptor acceptor;
};

void Server::start_accept() {
    boost::shared_ptr<Connection> connection(new Connection(io_service));
    acceptor.async_accept(connection->socket(), [this, connection](const boost::system::error_code &error_code) {
        this->handle(connection, error_code);
    });
}

int main(int argc, char *argv[]) {
//    int rank, size;
//    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    try {
        const unsigned short port = (argc > 1 ? static_cast<unsigned short>(std::stoi(argv[1])) : 1234);

        boost::asio::io_service io_service;
        tcp::endpoint ep{tcp::v4(), port};
        Server server(io_service, ep);

        server.start_accept();
        io_service.run();
    }
    catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;

}

