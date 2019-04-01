#include "signupwind.h"
#include "ui_signupwind.h"
#include "welcomewindow.h"
#include "utils.h"

SignUpWind::SignUpWind(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SignUpWind)
{
    ui->setupUi(this);
    ui->label_6->setText("");
}

SignUpWind::~SignUpWind()
{
    delete ui;
}

void SignUpWind::on_pushButton_clicked()
{
    QString firstName, secondName, username, password, confirmPassword;
    firstName=ui->lineEdit->text();
    secondName=ui->lineEdit_2->text();
    username=ui->lineEdit_3->text();
    password=ui->lineEdit_4->text();
    confirmPassword=ui->lineEdit_5->text();

    char strFirstName[30], strSecondName[30], strUsername[30], strPassword[30], strConfirmPassword[30];

    QByteArray bFirstName=firstName.toLatin1();
    strcpy(strFirstName,bFirstName.data());

    QByteArray bSecondName=secondName.toLatin1();
    strcpy(strSecondName,bSecondName.data());

    QByteArray bUsername=username.toLatin1();
    strcpy(strUsername,bUsername.data());

    QByteArray bPassword=password.toLatin1();
    strcpy(strPassword,bPassword.data());

    QByteArray bConfirmPassword=confirmPassword.toLatin1();
    strcpy(strConfirmPassword,bConfirmPassword.data());

    QPalette sample_palette;
    sample_palette.setColor(QPalette::Window, Qt::white);
    sample_palette.setColor(QPalette::WindowText, Qt::red);
    ui->label_6->setAutoFillBackground(false);

    int credentialsLen;
    char *credentials = (char*)malloc(62);

    if((strlen(strFirstName)==0)||(strlen(strSecondName)==0)||(strlen(strUsername)==0)||(strlen(strPassword)==0)||(strlen(strConfirmPassword)==0)){
        ui->label_6->setPalette(sample_palette);
        ui->label_6->setText("You haven't fill all of the fields. Type again!");
    }//verificare daca campurile sunt umplute
    else
        if(strcmp(strConfirmPassword, strPassword)!=0){
            ui->label_6->setText("The passwords you have already typed are not the same. Type again!");
            ui->label_6->setPalette(sample_palette);
        }
        else {

            credentialsLen = strlen(strUsername) + strlen(strPassword) + 1;
            strcpy(credentials, strUsername);
            strcat(credentials, "$");
            strcat(credentials, strPassword);
            strcat(credentials, "$");

            int loginOrSignUp;
            loginOrSignUp = 2; //flag pentru a sti daca utilizatorul vrea sa se inregistreze

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
                qDebug()<<"[client]Eroare la read()-ul confirmarii pentru credentials de la server.\n";
            }
            //qDebug()<<confirmation;
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
                ui->label_6->setPalette(sample_palette);
                ui->label_6->setText("The username is already existent in the database. Type again!");
            }
        }
}
