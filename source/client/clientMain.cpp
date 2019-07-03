#include <boost/asio.hpp>
#include <iostream>
#include <fstream>
#include "client.hpp"

int main(int argc, char* argv[]) {

    if (argc < 3) {
        std::cerr << R"(Bad argument for connect to server, pattern: "ip" "port" "file name".)" << std::endl;
        return 0;
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

        while (true) {}

    } catch (const std::exception &ex) {
        std::cerr << "Error: " << ex.what();
    }
    return 0;
}
