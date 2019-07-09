#include "main_window.h"
#include "ui_main_window.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //ui->statusBar->hide();
    //ui->mainToolBar->hide();

    gl_widget = ui->openGLWidget;

    connect(gl_widget, &MyOpenGLWidget::initialized, this, &MainWindow::initGlWidget);
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::initGlWidget(){}
void MainWindow::on_actionPlanetary_triggered(){
    gl_widget->setPalette(makePlanetPalette());
    gl_widget->setShaderProgram(0);
    gl_widget->update();
}
void MainWindow::on_actionStellar_triggered(){
    gl_widget->setPalette(makeStarPalette());
    gl_widget->setShaderProgram(1);
    gl_widget->update();
}
