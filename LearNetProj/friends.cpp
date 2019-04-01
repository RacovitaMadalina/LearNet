#include "friends.h"
#include "ui_friends.h"
#include "utils.h"
#include <QDebug>
#include <string>

Friends::Friends(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Friends)
{
    ui->setupUi(this);
    ui->label_4->setText("");
    ui->label_6->setText("");
}

Friends::~Friends()
{
    delete ui;
}

void Friends::on_pushButton_2_clicked()
{
    hide();
}

void Friends::on_pushButton_3_clicked()
{
    int weChoseShowUsers=4; //am ales sa vedem lista de utilizatori
    if ( write(sd, &weChoseShowUsers , 4)  <= 0) {
        qDebug()<<"[client]Eroare la write()-ul optiunii Show Users pentru Friends Window spre server.\n";
    }

    QPalette sample_palette;
    sample_palette.setColor(QPalette::Window, Qt::white);
    sample_palette.setColor(QPalette::WindowText, Qt::red);
    ui->label_4->setAutoFillBackground(false);
    ui->label_4->setPalette(sample_palette);

    ui->label_4->setText("Let's see the current users!");
    ui->textEdit->setText("Userii curenti ce folosesc LearNet sunt::");
    ui->textEdit->append("*****************************");
    int numeLen;
    char* nume=(char*)malloc(30);
    int numberRec;

    //citim de la server numarul de mesaje aferente topicului
    if (read (sd, &numberRec,4) < 0) {
        qDebug()<<"[client]Eroare la read()-ul numarului useri (Friends Window) de la server.\n";
    }

    for(int i=0; i<numberRec; i++)
    {
        fflush(stdout);
        //citim lungimea numelui primit de la server
        if (read (sd, &numeLen,4) < 0) {
            qDebug()<<"[client]Eroare la read()-ul lungimii numelui (Friends Window) de la server.\n";
        }
        fflush(stdout);
        strcpy(nume, "");
        //citim username-ul care a scris mesajul de la server
        if (read (sd, nume,numeLen) < 0) {
            qDebug()<<"[client]Eroare la read()-ul username-ului (Friends Window) de la server.\n";
        }
        nume[numeLen]='\0';
        ui->textEdit->append(nume);
    }
}

void Friends::on_pushButton_4_clicked()
{
    int weChoseShowFriends=5; //am ales sa vedem lista de prieteni
    if ( write(sd, &weChoseShowFriends , 4)  <= 0) {
        qDebug()<<"[client]Eroare la write()-ul optiunii Show Users pentru Friends Window spre server.\n";
    }

    QPalette sample_palette;
    sample_palette.setColor(QPalette::Window, Qt::white);
    sample_palette.setColor(QPalette::WindowText, Qt::red);
    ui->label_4->setAutoFillBackground(false);
    ui->label_4->setPalette(sample_palette);

    ui->label_4->setText("Let's see your list of friends!");
    ui->textEdit_2->setText("Prietenii tai de pe LearNet sunt::");
    ui->textEdit_2->append("*****************************");
    int numeLen;
    char* nume=(char*)malloc(30);
    int numberRec;

    //citim de la server numarul de mesaje aferente topicului
    if (read (sd, &numberRec,4) < 0) {
        qDebug()<<"[client]Eroare la read()-ul numarului de prieteni (Friends Window) de la server.\n";
    }

    for(int i=0; i<numberRec; i++)
    {
        fflush(stdout);
        //citim lungimea numelui primit de la server
        if (read (sd, &numeLen,4) < 0) {
            qDebug()<<"[client]Eroare la read()-ul lungimii numelui prietenului (Friends Window) de la server.\n";
        }
        bzero(nume, numeLen);
        fflush(stdout);
        //citim username-ul care a scris mesajul de la server
        if (read (sd, nume,numeLen) < 0) {
            qDebug()<<"[client]Eroare la read()-ul username-ului prietenului (Friends Window) de la server.\n";
        }
        nume[numeLen]='\0';
        ui->textEdit_2->append(nume);
    }
}

void Friends::on_pushButton_clicked()
{
    QString optionFriend;
    optionFriend=ui->lineEdit->text();
    int confirmation; //confirmare primita de la server privitoare la corectitudinea numelui prietenului

    const char* charFriend;

    int weChoseAddFriend=6;
    int faVerifInServer;

    std::string strFriend=optionFriend.toStdString();
    charFriend=strFriend.c_str();

    QPalette sample_palette;
    sample_palette.setColor(QPalette::Window, Qt::white);
    sample_palette.setColor(QPalette::WindowText, Qt::red);
    ui->label_4->setAutoFillBackground(false);
    ui->label_4->setPalette(sample_palette);
    ui->label_6->setAutoFillBackground(false);
    ui->label_6->setPalette(sample_palette);
    ui->label_4->setText("");
    ui->label_6->setText("");

    int friendLen=strFriend.length();

    if ( write(sd, &weChoseAddFriend , 4)  <= 0) {
        qDebug()<<"[client]Eroare la write()-ul optiunii search/forum/friends pentru searchWindow spre server.\n";
    }

    //daca putem stabili corectitudinea fara a mai trimite la server, vom pune doar labeluri corespunzatoare, daca nu, setam faVerifInServer la 1
    if(strFriend.length()==0){
        ui->label_4->setText("You haven't type any username. Type again!");
        faVerifInServer=0;
        if ( write(sd, &faVerifInServer , 4)  <= 0) {
            qDebug()<<"[client]Eroare la write()-ul optiunii de fa verificarea in server pentru FriendsWindow spre server.\n";
        }
    }
    else
    {
        faVerifInServer=1;
        if ( write(sd, &faVerifInServer , 4)  <= 0) {
            qDebug()<<"[client]Eroare la write()-ul optiunii de fa verificarea in server pentru FriendsWindow spre server.\n";
        }
        //scriem in server lungimea numelui
        fflush(stdout);
        if ( write(sd, &friendLen , 4)  <= 0) {
            qDebug()<<"[client]Eroare la write()-ul optiunii de topic pentru searchWindow spre server.\n";
        }
        //scriem in server numele
        fflush(stdout);
        if ( write(sd, charFriend , strFriend.length())  <= 0) {
            qDebug()<<"[client]Eroare la write()-ul optiunii de topic pentru searchWindow spre server.\n";
        }
        //citim din server confirmarea privitoare la corectitudinea numelui
        fflush(stdout);
        if (read (sd, &confirmation,4) < 0) {
            qDebug()<<"[client]Eroare la read()-ul confirmarii pentru topic (searchWindow) de la server.\n";
        }
        if(confirmation == 0)
            ui->label_4->setText("The username is inexistent. Type again!");
        else if(confirmation == 2){
            ui->label_4->setText("The username you've typed is the ");
            ui->label_6->setText("same as yours! Type again!");
        }
        else if(confirmation == 3){
            ui->label_4->setText("The username you've typed is already");
            ui->label_6->setText("your friend. Type again!");
        }
        else{
            ui->label_4->setText("Well done! Press Show list of friends");
            ui->label_6->setText("to see the updated list.");
        }
    }
}
