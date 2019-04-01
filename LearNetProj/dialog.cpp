#include "dialog.h"
#include "ui_dialog.h"
#include "signupwind.h"
#include "welcomewindow.h"
#include<QString>
#include "utils.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    ui->label_3->setText("");
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_pushButton_clicked()
{
    QString user, passwd;
    user=ui->lineEdit->text();
    passwd=ui->lineEdit_2->text();

    char strUser[30], strPasswd[255];

    QByteArray bUser=user.toLatin1();
    strcpy(strUser,bUser.data());

    QByteArray bPasswd=passwd.toLatin1();
    strcpy(strPasswd,bPasswd.data());

    QPalette sample_palette;
    sample_palette.setColor(QPalette::Window, Qt::white);
    sample_palette.setColor(QPalette::WindowText, Qt::red);
    ui->label_3->setAutoFillBackground(false);

    int credentialsLen;
    char *credentials = (char*)malloc(62);

    if((strlen(strUser)==0)||(strlen(strPasswd)==0)){
        ui->label_3->setPalette(sample_palette);
        ui->label_3->setText("You haven't fill all of the fields. Type again!");
    }//verificare daca campurile sunt umplute
    else {

        credentialsLen = strlen(strUser) + strlen(strPasswd) + 2;

        strcpy(credentials, strUser);
        strcat(credentials, "$");
        strcat(credentials, strPasswd);
        strcat(credentials, "$");

        int loginOrSignUp;
        loginOrSignUp = 1; //flag pentru a sti daca utilizatorul vrea sa se loggeze

        if ( write(sd, &loginOrSignUp, 4)  <= 0) {
            qDebug()<<"[client]Eroare la write()-ul optiunii loginOrSignUp spre server.\n";
        }

        //trimitem la server lungimea si stringul in care am concatenat username si pass;
        if ( write(sd, &credentialsLen, 4)  <= 0) {
            qDebug()<<"[client]Eroare la write()-ul dimensiunii string-ului pentru credentials spre server.\n";
        }
        fflush(stdout);
        if ( write(sd, credentials, credentialsLen) <= 0) {
            qDebug()<<"[client]Eroare la write()-ul stringului de crdenetials spre server.\n";
        }

        /* citirea raspunsului dat de server (confirmare la Login)
         (apel blocant pina cind serverul raspunde) */
        int confirmation;

        //fflush (stdout);

        if (read (sd, &confirmation,sizeof(int)) < 0) {
            qDebug()<<"[client]Eroare la read()-ul confirmarii pentru credentials spre server.\n";
        }
        qDebug()<<confirmation;
        if (confirmation==1)//logarea s-a realizat cu succes
        {
            hide();
            WelcomeWindow WelcomeWindow;
            WelcomeWindow.setModal(true);
            WelcomeWindow.exec();
            //modal approach
        }
        else
        {
            ui->label_3->setPalette(sample_palette);
            ui->label_3->setText("The username or the password you've already typed is wrong. Type again!");
            //Dialog::on_pushButton_clicked();
        }
    }
}

void Dialog::on_pushButton_2_clicked()
{
    hide();
    SignUpWind SignUpWindow;
    SignUpWindow.setModal(true);
    SignUpWindow.exec();
    //modal approach

}


