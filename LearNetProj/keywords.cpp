#include "keywords.h"
#include "ui_keywords.h"
#include "utils.h"
#include <string>

char* intToChar(int x)
{
    int number=x;
    int reverse=0;
    std::string p;

    while(number)
    {
        reverse=reverse*10+number%10;
        number=number/10;
    }

    number=reverse;
    while(number)
    {
        p.push_back(char('0'+number%10));
        number=number/10;
    }
    char* cstr=strdup(p.c_str());
    return cstr;
}

Keywords::Keywords(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Keywords)
{
    ui->setupUi(this);
    ui->label_3->setText("");
}

Keywords::~Keywords()
{
    delete ui;
}

void Keywords::on_pushButton_clicked()
{
    hide();
}

void Keywords::on_pushButton_2_clicked()
{
    //am ales sa Keywords Search
    QPalette sample_palette;
    sample_palette.setColor(QPalette::Window, Qt::white);
    sample_palette.setColor(QPalette::WindowText, Qt::red);
    ui->label_3->setAutoFillBackground(false);
    ui->label_3->setPalette(sample_palette);

    const char* charKeyword;
    QString keyword;
    keyword=ui->lineEdit->text();
    std::string str=keyword.toStdString();
    charKeyword=str.c_str();

    if(str.length()==0){
        ui->label_3->setText("You have to type a keyword!");
        int weChoseKeyword=0;//trimitem o optiune neutra deoarece clientul trebuie sa introduca un keyword si sa apese din nou pe search
        if ( write(sd, &weChoseKeyword , 4)  <= 0) {
            qDebug()<<"[client]Eroare la write()-ul optiunii weChoseKeyword spre server.\n";
        }
    }
    else{
        int weChoseKeyword=9;
        if ( write(sd, &weChoseKeyword , 4)  <= 0) {
            qDebug()<<"[client]Eroare la write()-ul optiunii weChoseKeyword spre server.\n";
        }

        int keywordLen=str.length();
        //trimitem la server keyword-ul, mai intai lungimea sa, si dupa keyword-ul propriu zis
        if ( write(sd, &keywordLen , 4)  <= 0) {
            qDebug()<<"[client]Eroare la write()-ul lungimii keyword-ului spre server.\n";
        }

        if ( write(sd, charKeyword , keywordLen)  <= 0) {
            qDebug()<<"[client]Eroare la write()-ul keyword-ului spre server.\n";
        }

        int nrTopics=0;
        fflush(stdout);
        if (read (sd, &nrTopics,sizeof(int)) < 0) {
            qDebug()<<"[client]Eroare la read()-ul numarului de topicuri in care am gasit keyword-ul de la server.\n";
        }

        char* topic=(char*)malloc(30);
        int topicLen;
        int occurrences;
        char* text=(char*)malloc(100);

        if(nrTopics==0)
        {
            ui->textEdit->setText("No results found for this keyword");
            ui->textEdit->append("*****************************");
        }
        else
        {
            ui->label_3->setText("Let's see the current topics!");
            ui->textEdit->setText("The topics in which we found the keyword given are:");
            ui->textEdit->append("*****************************");
        }

        for(int i=0; i<nrTopics; i++)
        {
            strcpy(text, "");
            fflush(stdout);

            //citim lungimea numelui  topicului primit de la server
            if (read (sd, &topicLen,4) < 0) {
                qDebug()<<"[client]Eroare la read()-ul lungimii topicului (keywords search) de la server.\n";
            }

            fflush(stdout);
            //citim numele topicului primit de la server
            if (read (sd, topic,topicLen) < 0) {
                qDebug()<<"[client]Eroare la read()-ul topicului (keywords search) de la server.\n";
            }
            topic[topicLen]='\0';

            //citim numarul de aparitii ale keyword-ului de la server
            if (read (sd, &occurrences,4) < 0) {
                qDebug()<<"[client]Eroare la read()-ul numarului de aparitii ale keyword-ului de la server.\n";
            }

            ui->textEdit->append("*********************************");
            char* aux=(char*)malloc(5);

            topic[topicLen]='\0';
            strcat(text, "In capitolul: ");
            strcat(text, topic);
            strcat(text, "---->am gasit ");
            strcat(text, intToChar(occurrences));
            strcat(text, " sugestii");
            ui->textEdit->append(text);
        }

    }
}
