#include "main_window.h"
#include "ui_main_window.h"
#include "map_dialog.h"

MainWindow::MainWindow(QWidget *parent, Client *client) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    client(client)
{
    ui->setupUi(this);

    //ui->statusBar->hide();
    //ui->mainToolBar->hide();

    gl_widget = ui->openGLWidget;

    ui->openGLWidget->set_client(client);

    connect(gl_widget, &MyOpenGLWidget::initialized, this, &MainWindow::initGlWidget);
}

void MainWindow::deleteQDialog() {
    client->delete_deque_map();
    delete dlg;
    dlg = nullptr;
}

MainWindow::~MainWindow() {
    delete dlg;
    delete ui;
    delete client;
}

void MainWindow::initGlWidget() {}

void MainWindow::on_actionStart_triggered() {
    client->start();
}

void MainWindow::on_actionPause_triggered() {
    client->pause();
}

void MainWindow::on_actionNext_triggered() {
    client->resume();
}

void MainWindow::on_actionStop_triggered() {
    client->stop();
}

void MainWindow::on_actionPlanetary_triggered(){
    isStellar = false;
    if (dlg) {
        dynamic_cast<MapDialog*>(dlg)->resize_dist_max(isStellar);
    }
    gl_widget->setPalette(makePlanetPalette());
    gl_widget->setShaderProgram(false);
    gl_widget->update();
}
void MainWindow::on_actionStellar_triggered(){
    isStellar = true;
    if (dlg) {
        dynamic_cast<MapDialog*>(dlg)->resize_dist_max(isStellar);
    }
    gl_widget->setPalette(makeStarPalette());
    gl_widget->setShaderProgram(true);
    gl_widget->update();
}
void MainWindow::on_actionDensity_triggered(){
    if (!dlg) {
        dlg = new MapDialog(this, client) /*std::make_unique<MapDialog>(this)*/;
        dynamic_cast<MapDialog*>(dlg)->resize_dist_max(isStellar);
        dlg->setModal(false);
        dlg->show();
    }
//    dlg->exec();
//    if (dlg.exec() == QDialog::Accepted) {}
}
