#include "welcomewindow.h"
#include "ui_welcomewindow.h"
#include "searchwindow.h"

WelcomeWindow::WelcomeWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WelcomeWindow)
{   
    ui->setupUi(this);
    QPixmap picture("/home/madalina/Desktop/LearNet/img1.jpg");
    ui->label->setPixmap(picture);
    QPalette sample_palette;
    sample_palette.setColor(QPalette::Window, Qt::white);
    sample_palette.setColor(QPalette::WindowText, Qt::white);
    ui->label_2->setAutoFillBackground(false);
    ui->label_2->setPalette(sample_palette);
    ui->label_2->setText("Welcome to LearNet ! Press continue to go to the main page !");

}

WelcomeWindow::~WelcomeWindow()
{
    delete ui;
}

void WelcomeWindow::on_pushButton_clicked()
{
    hide();
    SearchWindow SearchWindow;
    SearchWindow.setModal(true);
    SearchWindow.exec();
    //modal approach
}
