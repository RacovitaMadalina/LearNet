#include "forum.h"
#include "ui_forum.h"
#include "friends.h"
#include "utils.h"

Forum::Forum(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Forum)
{
    ui->setupUi(this);
    ui->label_4->setText("");
}

Forum::~Forum()
{
    delete ui;
}

void Forum::on_pushButton_clicked()
{
    //am ales sa adaugam un mesaj
    QPalette sample_palette;
    sample_palette.setColor(QPalette::Window, Qt::white);
    sample_palette.setColor(QPalette::WindowText, Qt::red);
    ui->label_4->setAutoFillBackground(false);
    ui->label_4->setPalette(sample_palette);

    const char* strOption;
    QString optionTopic;
    optionTopic=ui->lineEdit_2->text();
    std::string str=optionTopic.toStdString();
    strOption=str.c_str();

    if(str.length()==0){
        ui->label_4->setText("You have to choose firstly a number of topic.Press Show forum!");
        int weChoseMessage=0;//trimitem o optiune neutra deoarece clientul trebuie sa apese peshow forum mai intai
        if ( write(sd, &weChoseMessage , 4)  <= 0) {
            qDebug()<<"[client]Eroare la write()-ul optiunii message pentru forum spre server.\n";
        }
    }
    else{
        int weChoseMessage=3;
        if ( write(sd, &weChoseMessage , 4)  <= 0) {
            qDebug()<<"[client]Eroare la write()-ul optiunii message pentru forum spre server.\n";
        }

        //trimitem la server numarul topicului ca sa stie carui topic sa ii adauge mesajul
        if ( write(sd, strOption , 2)  <= 0) {
            qDebug()<<"[client]Eroare la write()-ul optiunii de topic pentru forum spre server.\n";
        }

        const char* mesaj;
        QString message;
        message=ui->lineEdit->text();
        std::string strMess=message.toStdString();
        mesaj=strMess.c_str();

        int messLen=strMess.length();

        //trimitem la client lungimea mesajului
        if (write(sd,&messLen,4) <=0)
        {
            qDebug()<<"[client]Eroare la write()-ul lungimii mesajului pentru Forum catre server.\n";
        }

        //trimitem mesajul propriu-zis la server
        if (write(sd,mesaj,messLen) <=0)
        {
            qDebug()<<"[client]Eroare la write()-ul mesajului pentru Forum catre server.\n";
        }

        int confirmInsert;

        //citim de la server confirmarea daca mesajul a fost sau nu inserat
        if (read (sd, &confirmInsert,4) < 0) {
            qDebug()<<"[client]Eroare la read()-ul confirmarii insertului de mesaj (Forum) de la server.\n";
        }
        ui->label_4->setText("Press again Show forum to update the forum!");
    }
}

void Forum::on_pushButton_3_clicked()
{
    Friends friends;
    friends.setModal(true);
    friends.exec();
    //modal approach
}

void Forum::on_pushButton_2_clicked()
{
    hide();
}

void Forum::on_pushButton_4_clicked()
{
    QString optionTopic;
    optionTopic=ui->lineEdit_2->text();
    int confirmation; //confirmare primita de la server privitoare la corectitudinea numarului de topic

    const char* strOption;

    int cifra1, cifra2, numberOfTopic;
    int weChoseForum=2;
    int faVerifInServer;

    std::string str=optionTopic.toStdString();
    strOption=str.c_str();

    QPalette sample_palette;
    sample_palette.setColor(QPalette::Window, Qt::white);
    sample_palette.setColor(QPalette::WindowText, Qt::red);
    ui->label_4->setAutoFillBackground(false);
    ui->label_4->setPalette(sample_palette);

    if ( write(sd, &weChoseForum , 4)  <= 0) {
        qDebug()<<"[client]Eroare la write()-ul optiunii search/forum/friends pentru forum spre server.\n";
    }

    //daca putem stabili corectitudinea fara a mai trimite la server, vom pune doar labeluri corespunzatoare, daca nu, setam faVerifInServer la 1
    if(str.length()==0){
        ui->label_4->setText("You haven't type any option. Type again!");
        faVerifInServer=0;
        if ( write(sd, &faVerifInServer , 4)  <= 0) {
            qDebug()<<"[client]Eroare la write()-ul optiunii de fa verificarea in server pentru forum spre server.\n";
        }
    }
    else
        if(str.length()>2) {
            ui->label_4->setText("Incorrect number of the topic(too long). Type again!");
            faVerifInServer=0;
            if ( write(sd, &faVerifInServer , 4)  <= 0) {
                qDebug()<<"[client]Eroare la write()-ul optiunii de fa verificarea in server pentru forum spre server.\n";
            }
        }
        else
        {
            faVerifInServer=1;
            if ( write(sd, &faVerifInServer , 4)  <= 0) {
                qDebug()<<"[client]Eroare la write()-ul optiunii de fa verificarea in server pentru forum spre server.\n";
            }
            fflush(stdout);
            if ( write(sd, strOption , 2)  <= 0) {
                qDebug()<<"[client]Eroare la write()-ul optiunii de topic pentru forum spre server.\n";
            }
            fflush(stdout);
            if (read (sd, &confirmation,4) < 0) {
                qDebug()<<"[client]Eroare la read()-ul confirmarii pentru topic (forum) de la server.\n";
            }
            if(confirmation == 0)
                ui->label_4->setText("The number of the topic is inexistent. Type again!");
            else{
                ui->label_4->setText("Let's see the messages!");
                ui->textEdit_2->setText("Mesajele aferente topicului sunt:");
                int numeLen, mesajLen;
                char* nume=(char*)malloc(30);
                char* mesaj=(char*)malloc(300);
                int numberRec;

                //citim de la server numarul de mesaje aferente topicului
                if (read (sd, &numberRec,4) < 0) {
                    qDebug()<<"[client]Eroare la read()-ul numarului de mesaje aferente topicului (Forum) de la server.\n";
                }

                for(int i=0; i<numberRec; i++)
                {
                    //citim lungimea numelui primit de la server
                    if (read (sd, &numeLen,4) < 0) {
                        qDebug()<<"[client]Eroare la read()-ul lungimii numelui (Forum) de la server.\n";
                    }

                    //citim username-ul care a scris mesajul de la server
                    if (read (sd, nume,numeLen) < 0) {
                        qDebug()<<"[client]Eroare la read()-ul username-ului (Forum) de la server.\n";
                    }
                    nume[numeLen]='\0';
                    strcat(nume, ":");
                    ui->textEdit_2->append("**************************************");
                    ui->textEdit_2->append(nume);

                    //citim lungimea mesajului primit de la server
                    if (read (sd, &mesajLen,4) < 0) {
                        qDebug()<<"[client]Eroare la read()-ul lungimii mesajului (Forum) de la server.\n";
                    }

                    //citim mesajul de la server
                    if (read (sd, mesaj,mesajLen) < 0) {
                        qDebug()<<"[client]Eroare la read()-ul mesajului (Forum) de la server.\n";
                    }
                    mesaj[mesajLen]='\0';
                    ui->textEdit_2->append(mesaj);
                }
            }
        }
}
