#include "mainwindow.h"
#include "CImg.h"
#include "ui_mainwindow.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{


    str = QFileDialog::getOpenFileName(this, "Выбрать изображение", "C:\\",
                                       "PNG Image (*.png);; JPEG Image (*.jpg);");
    if (not str.isNull()) {
        try
        {
            changedimg_dir = (QCoreApplication::applicationDirPath().toStdString() + "/changed_img.png");
            image.load(str.toStdString().c_str());
            pix.load(str);
            pixwidth = pix.width();
            pixheight = pix.height();
            ui->label->setPixmap(pix);
        }
        catch (CImgIOException)
        {
            QMessageBox::critical(this, "Ошибка", "Директория файла должна содержать только латинские символы!");
        }
    }
}

void MainWindow::on_doubleSpinBox_valueChanged(double arg1)
{
    if (not pix.isNull() and (arg1 * ui->label->pixmap()->width() < 750) and (arg1 * ui->label->pixmap()->height() < 750)) {
        ui->label->setFixedWidth(arg1 * ui->label->pixmap()->width());
        ui->label->setFixedHeight(arg1 * ui->label->pixmap()->height());
    }
}

void BitsSwap(int column, int raw, int part, int symb) {
    int Red = (int)image(column, raw, 0, 0);
    int Green = (int)image(column, raw, 0, 1);
    int Blue = (int)image(column, raw, 0, 2);
    switch (part) {
    case 1:
        Red = (Red & 254) | (symb & 128) >> 7;
        Green = (Green & 254) | (symb & 64) >> 6;
        Blue = (Blue & 254) | (symb & 32) >> 5;
        break;
    case 2:
        Red = (Red & 254) | (symb & 16) >> 4;
        Green = (Green & 254) | (symb & 8) >> 3;
        Blue = (Blue & 254) | (symb & 4) >> 2;
        break;
    case 3:
        Red = (Red & 254) | (symb & 2) >> 1;
        Green = (Green & 254) | (symb & 1);
        break;
    }
    int color[] = { Red, Green, Blue };
    image.draw_point(column, raw, color);
}

string GetBits(int column, int raw, int part, string ans) {
    int Red = (int)image(column, raw, 0, 0);
    int Green = (int)image(column, raw, 0, 1);
    int Blue = (int)image(column, raw, 0, 2);
    if (part == 3) {
        //ans *= 10;
        ans += to_string(Red & 1);
        //ans *= 10;
        ans += to_string(Green & 1);
        return ans;
    }
    //ans *= 10;
    ans += to_string(Red & 1);
    //ans *= 10;
    ans += to_string(Green & 1);
    //ans *= 10;
    ans += to_string(Blue & 1);
    return ans;
}

string ToBinary(unsigned int val, unsigned int power) {
/* � азбивает исходный Байт на биты. Возвращает массив из восьми элементов
   Каждый Nй элемент соответствует Nму биту.
   Принимает значения 1 или 0
*/
    int Result = val;
    if (power != 10) {
        Result = 0;
        for (int i = 0; val > 0; i++) {
            Result += val % 10 * pow(power, i);
            val /= 10;
        }
    }
    string ans;
    for (int i = 0; i < 8; i++) {
        ans += char('0' + Result % 2);
        Result /= 2;
    }
    return string(ans.crbegin(), ans.crend());
}

int BinToDec(string val) {
    int ans = 0;
    for (int i = 0; i < val.length(); i++) {
        ans += ((val[i]-'0') * pow(2, (val.length() - i - 1)));

    }
    return ans;
}

void MainWindow::on_pushButton_3_clicked()
{
    if (pix.isNull()){
        QMessageBox::critical(this, "Ошибка", "Сперва загрузите изображение!");
        return;
    }
    //image.display();
    int step = ui->spinBox->value();
    int messageLen = ui->lineEdit->text().length();
    int max_messageLen = (pixwidth * pixheight - 9) / (step + 3);
    int k = 0;
    int row = 0;
    int column = 0;
    int i;
    int ost;
    k = messageLen*2;
    unsigned char message[k];
    for (int i = 0; i < k; i++){
        message[i] = 0;
    }
    strcpy((char*)message, ui->lineEdit->text().toStdString().c_str());
    for (int i = 0; i < k; i++){
        if ((int)message[i] > 127){
            messageLen++;
            i++;
        }
    }
    if (messageLen > max_messageLen){
        QMessageBox::critical(this, "Ошибка", "Сообщение не влезет в выбранное изображение!\nСократите сообщение, выберите изображение большего разрешения или понизьте шаг кодирования.");
        return;
    }
    k = 0;

    for (row = 0; row < pixheight; row++) {
        for (column = 0; column < pixwidth; column++) {

            if (k == 6) {
                k++;
                break;
            }
            else if (k <= 2) {
                BitsSwap(column, row, (k - 3 * (k / 3) + 1), (int)'/');
                k++;
            }
            else if (k > 2 and k <= 5) {
                BitsSwap(column, row, (k - 3 * (k / 3) + 1), step);
                k++;
            }
        }
        if (k == 7){
            k = 0;
            i = k / 3;
            ost = step;
            break;
        }
    }
    //image.display();
    for (row; row < pixheight; row++) {
        for (column; column < pixwidth; column++) {
            if (i == messageLen) {
                i++;
                break;
            }
            BitsSwap(column, row, (k - 3 * (k / 3) + 1), (int)message[i]);
            //image.display();
            k++;
            if (k / 3 != i) {
                ost = step;
                while ((column + ost + 1) > (pixwidth - 1)) {
                    ost -= pixwidth - column;
                    row += 1;
                    column = 0;
                }
                column += ost;
            }
            i = k / 3;
        }
        if (i == messageLen+1) {
            k = 0;
            break;
        }
        if (column == pixwidth) column = 0;
    }

    for (row; row < pixheight; row++) {
        for (column; column < pixwidth; column++) {
            if (k == 3) {
                image.save(changedimg_dir.c_str());
                QMessageBox::information(this, "Выполнено", ("Файл изображения с закодированным сообщением сохранен в директории:\n"+changedimg_dir).c_str());
                image.load(str.toStdString().c_str());
                //changed_image.load(changedimg_dir.c_str());
                //changed_image.display();
                return;
            }
            else {
                BitsSwap(column, row, (k - 3 * (k / 3) + 1), 0);
                k++;
            }
        }
    }
    //image.load(str.toStdString().c_str());

}

void MainWindow::on_pushButton_2_clicked()
{
    if (pix.isNull()){
        QMessageBox::critical(this, "Ошибка", "Сперва загрузите изображение!");
        return;
    }
    //changed_image.display();
    int k = 0;
    int row = 0;
    int column = 0;
    int i;
    int ost;
    string key;
    string step_;
    string message;
    string symb;

    for (row = 0; row < pixheight; row++) {
        for (column = 0; column < pixwidth; column++) {
            if (k == 6) {
                step = BinToDec(step_);
                k++;
                break;
            }

            if (k <= 2) {
                key = GetBits(column, row, (k - 3 * (k / 3) + 1), key);
                if (k == 2 and key != ToBinary((int)'/', 10)) {
                    QMessageBox::critical(this, "Ошибка", "В изображение не закодировано сообщение!");
                    return;
                };
                k++;
            }

            else if (k > 2 and k <= 5) {
                step_ = GetBits(column, row, (k - 3 * (k / 3) + 1), step_);
                k++;
            }
        }
        //if (k == 2 and key != ToBinary((int)'/', 10)) {
            //break;
        //}
        if (k == 7) {
            k = 0;
            i = k / 3;
            ost = step;
            break;
        }
    }

    for (row; row < pixheight; row++) {
        for (column; column < pixwidth; column++) {
            if ((k - 3 * (k / 3) + 1 == 1) and (k != 0) and (BinToDec(symb) != 0)) {
                message += BinToDec(symb);
                symb = "";
            }
            symb += GetBits(column, row, (k - 3 * (k / 3) + 1), "");
            //image.display();
            k++;
            if (k / 3 != i) {
                ost = step;
                while ((column + ost + 1) > (pixwidth - 1)) {
                    ost -= pixwidth - column;
                    row += 1;
                    column = 0;

                }
                column += ost;
            }
            i = k / 3;
            if ((k - 3 * (k / 3) + 1 == 1) and (k != 0) and (BinToDec(symb) == 0)) {
                break;
            }
        }
        if ((k - 3 * (k / 3) + 1 == 1) and (k != 0) and (BinToDec(symb) == 0)) {
            break;
        }
        else if (column == pixwidth) column = 0;
    }

    QString qmessage = QString::fromStdString(message);
    ui->textEdit->setText(qmessage);

}
