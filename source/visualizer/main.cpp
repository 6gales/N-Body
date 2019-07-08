#include "../../../N-Body/source/client/client.hpp"
#include <boost/asio.hpp>

#include "main_window.h"
#include <QApplication>
#include <boost/bind.hpp>
#include <iostream>
#include <fstream>


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

        MainWindow w{};
        w.start_client(host, port, data_file);
        w.setWindowTitle("N-Body Program");
        w.show();
        data_file.close();
        return a.exec();
    } catch (const std::exception &ex) {
        std::cerr << "Error: " << ex.what();
    }
    return 0;

}
