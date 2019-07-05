#include "../../client/client.hpp"
#include <boost/asio.hpp>

#include "main_window.h"
#include <QApplication>
#include <iostream>
#include <fstream>


int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    if (argc < 3) {
        std::cerr << R"(Bad argument for connect to server, pattern: "ip" "port" "file name".)" << std::endl;
        return 0;
    }
    try {
        const auto host = std::string(argv[1]);
        const auto port = static_cast<const unsigned short>(std::stoi(argv[2]));
        const auto file_name = std::string{"C:\\ssd.sccc\\N-Body\\source\\visualizer\\in.txt"};

        std::ifstream data_file{file_name};

        boost::asio::io_service io_service;

        std::shared_ptr<Client> client{new Client{io_service, std::make_shared<ParticlesDeque>()}};

        client->connect(host, port);
//        client->start(data_file);
        MainWindow w{};
        w.start_client(client, data_file);
        w.setWindowTitle("N-Body Program");
        w.show();

    } catch (const std::exception &ex) {
        std::cerr << "Error: " << ex.what();
    }

    return a.exec();
}
