#include "searchwindow.h"
#include "ui_searchwindow.h"
#include "forum.h"
#include <QDebug>
#include <QString>
#include "utils.h"
#include <string>
#include <QtGlobal>
#include "exit.h"
#include "keywords.h"

SearchWindow::SearchWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SearchWindow)
{
    ui->setupUi(this);
    ui->label_4->setText("");
}

SearchWindow::~SearchWindow()
{
    delete ui;
}

void SearchWindow::on_pushButton_2_clicked()
{
    Forum ForumWindow;
    ForumWindow.setModal(true);
    ForumWindow.exec();
    //modal approach
}

void SearchWindow::on_pushButton_clicked()
{
    QString optionTopic;
    optionTopic=ui->lineEdit->text();
    int confirmation; //confirmare primita de la server privitoare la corectitudinea numarului de topic

    const char* strOption;
    int cifra1, cifra2, numberOfTopic;
    int weChoseSearch=1;
    int faVerifInServer;
    char* charInformation=(char*)malloc(1350);

    std::string str=optionTopic.toStdString();
    strOption=str.c_str();

    QPalette sample_palette;
    sample_palette.setColor(QPalette::Window, Qt::white);
    sample_palette.setColor(QPalette::WindowText, Qt::red);
    ui->label_4->setAutoFillBackground(false);
    ui->label_4->setPalette(sample_palette);

    if ( write(sd, &weChoseSearch , 4)  <= 0) {
        qDebug()<<"[client]Eroare la write()-ul optiunii search/forum/friends pentru searchWindow spre server.\n";
    }

    //daca putem stabili corectitudinea fara a mai trimite la server, vom pune doar labeluri corespunzatoare, daca nu, setam faVerifInServer la 1
    if(str.length()==0){
        ui->label_4->setText("You haven't type any option. Type again!");
        faVerifInServer=0;
        if ( write(sd, &faVerifInServer , 4)  <= 0) {
            qDebug()<<"[client]Eroare la write()-ul optiunii de fa verificarea in server pentru searchWindow spre server.\n";
        }
    }
    else
        if(str.length()>2) {
            ui->label_4->setText("Incorrect number of the topic(too long). Type again!");
            faVerifInServer=0;
            if ( write(sd, &faVerifInServer , 4)  <= 0) {
                qDebug()<<"[client]Eroare la write()-ul optiunii de fa verificarea in server pentru searchWindow spre server.\n";
            }
        }
        else
        {
            faVerifInServer=1;
            if ( write(sd, &faVerifInServer , 4)  <= 0) {
                qDebug()<<"[client]Eroare la write()-ul optiunii de fa verificarea in server pentru searchWindow spre server.\n";
            }
            fflush(stdout);
            if ( write(sd, strOption , 2)  <= 0) {
                qDebug()<<"[client]Eroare la write()-ul optiunii de topic pentru searchWindow spre server.\n";
            }
            fflush(stdout);
            if (read (sd, &confirmation,4) < 0) {
                qDebug()<<"[client]Eroare la read()-ul confirmarii pentru topic (searchWindow) de la server.\n";
            }
            if(confirmation == 0)
                ui->label_4->setText("The number of the topic is inexistent. Type again!");
            else{
                ui->label_4->setText("Good choice!");

                int lenInformation;

                //citim lungimea informatiei primita de la server
                if (read (sd, &lenInformation,4) < 0) {
                    qDebug()<<"[client]Eroare la read()-ul lungimii informatiei aferente topicului (searchWindow) de la server.\n";
                }

                //citim de la server toata informatia
                if (read (sd, charInformation,lenInformation) < 0) {
                    qDebug()<<"[client]Eroare la read()-ul informatiei aferente topicului (searchWindow) de la server.\n";
                }
                charInformation[lenInformation]='\0';
                ui->textEdit_2->setText(charInformation);
            }
        }
}

void SearchWindow::on_pushButton_3_clicked()
{
    hide();
    Exit Exit;
    Exit.setModal(true);
    Exit.exec();
    //modal approach
}

void SearchWindow::on_pushButton_4_clicked()
{
    Keywords Keywords;
    Keywords.setModal(true);
    Keywords.exec();
}
