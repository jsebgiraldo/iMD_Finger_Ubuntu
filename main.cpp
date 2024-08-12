#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle("IMD FAP Readers");
    w.setWindowIconText(" ");
    w.setWindowIcon(QIcon(":/img/fingerprint.ico"));
    w.showMaximized();
    return a.exec();
}
