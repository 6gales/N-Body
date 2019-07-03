#include "server.hpp"
#include <iostream>

using namespace boost::asio::ip;

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

