#include "../client/client.hpp"
#include <boost/asio.hpp>

#include "main_window.h"
#include <QApplication>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <iostream>
#include <fstream>

using namespace boost::asio::ip;

int main(int argc, char *argv[]) {
    if (argc < 3) {
        std::cerr << R"(Bad argument for connect to server, pattern: "ip" "port" "file name".)" << std::endl;
        return 0;
    }
    try {
        QApplication a(argc, argv);
        const auto host = std::string(argv[1]);
        const auto port = static_cast<const unsigned short>(std::stoi(argv[2]));
        const auto file_name = std::string{argv[3]};

        std::ifstream data_file{file_name};

        boost::asio::io_service io_service;

        tcp::resolver resolver(io_service);

        auto ep = resolver.resolve(host, std::to_string(port));


        Client *client = new Client{io_service};

        client->connect(ep, data_file);

        if (client->get_count() == 0) {
            delete client;
            std::cerr << "Bad value for N-body task" << std::endl;
            return 0;
        }

        MainWindow w{nullptr, client};
        w.setWindowTitle("N-Body Program");
        w.show();
        data_file.close();
        return a.exec();
    } catch (const std::exception &ex) {
        std::cerr << "Error: " << ex.what();
    }
    return 0;

}
