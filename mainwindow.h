#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QMessageBox>
#include <QTextCodec>
#include <QWidget>
#include <QFileDialog>
#include <QPixmap>
#include <QApplication>
#include <QGuiApplication>
#include "CImg.h"
#include <iostream>
#include <string>
#include <vector>
using namespace std;
using namespace cimg_library;
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE
static CImg<unsigned char> image;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    int pixwidth;
    int pixheight;
    int step;
    int messageLen;
    QString str;
    string changedimg_dir;
    QPixmap pix;



private slots:
    void on_pushButton_clicked();

    void on_doubleSpinBox_valueChanged(double arg1);

    void on_pushButton_3_clicked();


    void on_pushButton_2_clicked();

    void on_label_3_customContextMenuRequested(const QPoint &pos);

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
