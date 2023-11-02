#include "mainwindow.h"
#include "CImg.h"
#include <QApplication>
#include <iostream>
#include <string>
#include <QTextStream>
#include <QTextCodec>

using namespace std;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle("Stegano");
    w.show();
    return a.exec();
}
