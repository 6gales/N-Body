#ifndef MAP_DIALOG_H
#define MAP_DIALOG_H

#include <QDialog>
#include "main_window.h"
#include "../client/client.hpp"

namespace Ui {
class MapDialog;
}

class MapDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MapDialog(QWidget *parent = nullptr, Client *client = nullptr);
    ~MapDialog() override;

protected:
    void reject() override;

private:
    Ui::MapDialog *ui;
    Client *client;
};

#endif // MAP_DIALOG_H
