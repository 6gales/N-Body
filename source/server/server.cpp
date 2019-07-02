#include <boost/asio.hpp>
#include <mpi/mpi.h>

using namespace boost::asio::ip;

void start_message(const std::string &msg) {
    if (msg.size() < 5) return;
    int count_objects = (msg.at(5) << 24) | (msg.at(6) << 16) | (msg.at(7) << 8) | (msg.at(8));
    std::string objects_inf = msg.substr(9, msg.size()-9);
    for (int i = 0; i < count_objects; ++i) {
        float x;
        float y;
        float z;
        float mass;
        memcpy(&x, objects_inf.c_str() + i*4, sizeof(float));
        memcpy(&y, objects_inf.c_str() + (i+1)*4, sizeof(float));
        memcpy(&z, objects_inf.c_str() + (i+2)*4, sizeof(float));
        memcpy(&mass, objects_inf.c_str() + (i+3)*4, sizeof(float));
    }
}

void parse_message(const std::string &msg) {
    if (!msg.compare(0, 5, "START")) {

    } else if (!msg.compare(0, 4, "STOP")) {

    } else if (!msg.compare(0, 4, "NEXT")) {

    }
}

void read_message(tcp::socket &sock) {
    boost::asio::streambuf buf;
    boost::asio::read_until(sock, buf, "\r\n");
    std::string msg(boost::asio::buffers_begin(buf), boost::asio::buffers_end(buf));

}



class Server {
private:
    class Connection;

public:

    Server(boost::asio::io_service &io_service, tcp::endpoint &endpoint) : acceptor(io_service, endpoint),
                                                                           io_service(io_service) {}

    void startAccept();

    ~Server() = default;

private:

    class Connection {
    public:
        explicit Connection(boost::asio::io_service &io_service) : sock(io_service) { }

        tcp::socket& socket() {
            return sock;
        }

        void start() {
            std::thread thread([this] (){
                while (true) {

                }
            });
            thread.detach();
        }

    private:
        tcp::socket sock;
    };

    void handle(const boost::shared_ptr<Connection> &connection, const boost::system::error_code &error_code) {
        if (!error_code) {
            connection->start();
        }
        this->startAccept();
    }

    boost::asio::io_service &io_service;
    tcp::acceptor acceptor;
};

void Server::startAccept() {
    boost::shared_ptr<Connection> connection(new Connection(io_service));
    acceptor.async_accept(connection->socket(), [this, connection](const boost::system::error_code &error_code) {
        this->handle(connection, error_code);
    });
}

int main(int argc, char *argv[]) {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    return 0;
}

