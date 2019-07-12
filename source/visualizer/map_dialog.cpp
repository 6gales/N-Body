#include "map_dialog.h"
#include "ui_map_dialog.h"

MapDialog::MapDialog(QWidget *parent, Client *client) :
    QDialog(parent),
    ui(new Ui::MapDialog),
    client(client)
{
    ui->setupUi(this);
    ui->openGLWidget->set_client(client);
}

void MapDialog::reject() {
    QDialog::reject();
    dynamic_cast<MainWindow*>(this->parent())->deleteQDialog();
}

MapDialog::~MapDialog()
{
    delete ui;
}
