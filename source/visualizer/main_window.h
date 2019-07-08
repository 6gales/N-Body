#pragma once

#include "../../../N-Body/source/client/client.hpp"
#include <QMainWindow>
#include <memory>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void start_client(std::string host, unsigned short port, std::ifstream &data_file);

private slots:
    void on_actionAbout_triggered();

private:
    std::shared_ptr<Client> client;
    Ui::MainWindow *ui;
};
