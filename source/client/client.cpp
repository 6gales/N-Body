#include "clientReader.h"
#include <iostream>
#include <fstream>
#include <queue>

using namespace boost::asio::ip;

std::string parse_line(const std::string &line);

std::string read_message(tcp::socket &sock) {
    boost::asio::streambuf buf;
    ClientReader reader{buf};
    boost::asio::read(sock, buf, [&reader](const boost::system::error_code& err_code, size_t bytes) -> size_t {
        reader.check_count_objects(err_code, bytes);
    });
    std::string msg{boost::asio::buffers_begin(buf.data()), boost::asio::buffers_end(buf.data())};
    return msg;
}

void write_message(tcp::socket &sock, const std::string &msg) {
    boost::asio::write(sock, boost::asio::buffer(msg));
}

class Client {
public:

    explicit Client(boost::asio::io_service& io_service) : io_service(io_service), sock{io_service} {}

    tcp::socket& socket() {
        return sock;
    }

    void connect(const std::string &host, const unsigned short port) {
        tcp::resolver resolver(io_service);

        auto ep = resolver.resolve(host, std::to_string(port));
        boost::asio::connect(sock, ep);

        std::thread write_thread{[this]() {
            while (true) {
                std::unique_lock<std::mutex> lck(mutex);
                while (isEmptyQueue) cond_var.wait(lck);
                mutex.lock();
                const auto msg = msg_queue.front();
                msg_queue.pop();
                if (msg_queue.empty()) isEmptyQueue = true;
                mutex.unlock();
                write_message(this->socket(), msg);
            }
        }};
        write_thread.detach();

        std::thread read_thread{[this]() {
            while (true) {
                const auto msg = read_message(this->socket());
                //TODO convert msg to good format for visualisator
            }
        }};
        read_thread.detach();
    }

    void start(std::ifstream &data_file) {
        std::string start_message;
        start_message.append("START");
        size_t count = 0;
        start_message.append(reinterpret_cast<const char*>(&count), 8);
        while (data_file.good()) {
            std::string line;
            std::getline(data_file, line);
            std::string part_msg = parse_line(line);
            if (part_msg.empty()) {
                break;
            }

            start_message.append(part_msg);
            ++count;
        }

        for (size_t i = 0; i < 8; ++i) {
            start_message[5+i] = (char)(count >> ((7-i)*8));
        }

        msg_queue.push(start_message);
        isEmptyQueue = true;
        cond_var.notify_one();

    }

    void stop() {
        msg_queue.push(std::string{"STOP "});
        isEmptyQueue = true;
        cond_var.notify_one();
    }

    void next() {
        msg_queue.push(std::string{"NEXT "});
        isEmptyQueue = true;
        cond_var.notify_one();
    }

private:
    boost::asio::io_service &io_service;
    tcp::socket sock;
    std::mutex mutex;
    bool isEmptyQueue = true;
    std::queue<std::string> msg_queue{};
    std::condition_variable cond_var;
};

int main(int argc, char* argv[]) {

    if (argc < 3) {
        //TODO bad arguments
        return -1;
    }

    try {
        const auto host = std::string(argv[1]);
        const auto port = static_cast<const unsigned short>(std::stoi(argv[2]));
        const auto file_name = std::string{argv[3]};

        std::ifstream data_file{file_name};

        boost::asio::io_service io_service;

        Client client{io_service};

        client.connect(host, port);

        client.start(data_file);

    } catch (const std::exception &ex) {
        std::cerr << "Error: " << ex.what();
    }

    return 0;
}