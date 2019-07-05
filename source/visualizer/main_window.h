#pragma once

#include <../client/client.hpp>
#include <QMainWindow>
#include <memory>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void start_client(std::shared_ptr<Client>, std::ifstream& file);

private slots:
    void on_actionAbout_triggered();

private:
    std::shared_ptr<Client> client;
    Ui::MainWindow *ui;
};

