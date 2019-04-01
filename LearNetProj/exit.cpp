#include "exit.h"
#include "ui_exit.h"
#include "dialog.h"
#include "utils.h"

Exit::Exit(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Exit)
{
    ui->setupUi(this);
    QPixmap picture("/home/madalina/Desktop/LearNet/img2.jpg");
    ui->label->setPixmap(picture);
    QPalette sample_palette;
    sample_palette.setColor(QPalette::Window, Qt::white);
    sample_palette.setColor(QPalette::WindowText, Qt::white);
    ui->label_2->setAutoFillBackground(false);
    ui->label_2->setPalette(sample_palette);
    ui->label_2->setText("Thank you for choosing LearNet!");
}

Exit::~Exit()
{
    delete ui;
}

void Exit::on_pushButton_clicked()
{
    int weChoseExit=8; //am ales butonul de exit
    if ( write(sd, &weChoseExit , 4)  <= 0) {
        qDebug()<<"[client]Eroare la write()-ul optiunii Show Users pentru Friends Window spre server.\n";
    }
    hide();
}

void Exit::on_pushButton_2_clicked()
{
    int weChoseSignOut=7; //am ales butonul de sign out
    if ( write(sd, &weChoseSignOut , 4)  <= 0) {
        qDebug()<<"[client]Eroare la write()-ul optiunii Show Users pentru Friends Window spre server.\n";
    }
    hide();
    Dialog Dialog;
    Dialog.setModal(true);
    Dialog.exec();
}
