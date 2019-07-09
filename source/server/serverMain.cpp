#include "server.hpp"
#include <iostream>

using namespace boost::asio::ip;

int main(int argc, char *argv[]) {
    try {
        const auto ip_address = (argc > 1 ? std::string{argv[1]} : std::string{"0.0.0.0"});
        const unsigned short port = (argc > 2 ? static_cast<unsigned short>(std::stoi(argv[2])) : 1234);
        const size_t count_nodes = (argc > 3 ? std::stoull(argv[3]) : 1);

        boost::asio::io_service io_service;
        tcp::endpoint ep{address::from_string(ip_address), port};
        Server server(io_service, ep, count_nodes);

        server.start_working();
        io_service.run();
    }
    catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
