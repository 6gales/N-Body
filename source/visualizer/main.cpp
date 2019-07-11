#include "main_window.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    MainWindow w;
    w.setWindowTitle("N-Body Program");
    w.show();

    return a.exec();
}
