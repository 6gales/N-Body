#pragma once

#include "../client/client.hpp"
#include <QDialog>
#include "main_window.h"

namespace Ui {
class MapDialog;
}

class MapDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MapDialog(QWidget *parent = nullptr, Client *client = nullptr);
    void resize_dist_max(bool);
    ~MapDialog() override;

protected:
    void reject() override;

private:
    Ui::MapDialog *ui;
    Client *client;
};
