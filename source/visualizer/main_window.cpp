#include "main_window.h"
#include "ui_main_window.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //ui->statusBar->hide();
    //ui->mainToolBar->hide();
}

void MainWindow::start_client(std::string host, unsigned short port, std::ifstream &data_file) {
    ui->openGLWidget->set_client(host, port, data_file);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionAbout_triggered()
{

}
