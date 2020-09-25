#include "mainwindow.h"
#include <QApplication>

/* FUNÇÃO PRINCIPAL*/

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w; // cria o objeto MainWindow
    w.show();    // Roda o MainWindow, todo o restante do funcionamento está em mainwindow.cpp e mainwindow.h

    return a.exec();
}
