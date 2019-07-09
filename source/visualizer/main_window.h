#pragma once

#include <QMainWindow>
#include <QOpenGLFunctions>

namespace Ui {
class MainWindow;
}
class MyOpenGLWidget;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void initGlWidget();

    void on_actionPlanetary_triggered();
    void on_actionStellar_triggered();
private:
    Ui::MainWindow *ui;
    MyOpenGLWidget *gl_widget;
};

