#pragma once

#include <QMainWindow>
#include <QDialog>
#include <QOpenGLFunctions>
#include "../client/client.hpp"

namespace Ui {
class MainWindow;
}
class MyOpenGLWidget;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr, Client *client = nullptr);
    ~MainWindow();
    void deleteQDialog() {
        client->delete_deque_map();
        delete dlg;
        dlg = nullptr;
    }

private slots:
    void initGlWidget();

    void on_actionPlanetary_triggered();
    void on_actionStellar_triggered();
    void on_actionDensity_triggered();
    void on_actionStart_triggered();
    void on_actionStop_triggered();
    void on_actionPause_triggered();
    void on_actionNext_triggered();
private:
    QDialog *dlg = nullptr;
    Client *client;
    Ui::MainWindow *ui;
    MyOpenGLWidget *gl_widget;
};

