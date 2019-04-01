#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <QCoreApplication>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlError>
#include <QSqlQuery>
#include <sqlite3.h>
#include <QDebug>
#include "md5.h"
#include <string>

/*unsigned char *MD5(const unsigned char *d,  unsigned long n, unsigned char *md);*/

/* portul folosit */
#define PORT 2020

/* codul de eroare returnat de anumite apeluri */
extern int errno;

char* username = (char*) malloc (30);

QSqlDatabase db;

typedef struct thData{
    int idThread; //id-ul thread-ului tinut in evidenta de acest program
    int cl; //descriptorul intors de accept
}thData;

static void *treat(void *); /* functia executata de fiecare thread ce realizeaza comunicarea cu clientii */
void raspunde(void *);

int connectToDatabase()
{
    db =  QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("/home/madalina/Desktop/LearNet/LearNet.db");
    return 1;
}

int main ()
{

    struct sockaddr_in server;	// structura folosita de server
    struct sockaddr_in from;
    int nr;		//mesajul primit de trimis la client
    int sd;		//descriptorul de socket
    int pid;
    pthread_t th[100];    //Identificatorii thread-urilor care se vor crea
    int i=0;


    /* crearea unui socket */
    if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror ("[server]Eroare la socket().\n");
        return errno;
    }
    /* utilizarea optiunii SO_REUSEADDR */
    int on=1;
    setsockopt(sd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));

    /* pregatirea structurilor de date */
    bzero (&server, sizeof (server));
    bzero (&from, sizeof (from));

    /* umplem structura folosita de server */
    /* stabilirea familiei de socket-uri */
    server.sin_family = AF_INET;
    /* acceptam orice adresa */
    server.sin_addr.s_addr = htonl (INADDR_ANY);
    /* utilizam un port utilizator */
    server.sin_port = htons (PORT);

    /* atasam socketul */
    if (bind (sd, (struct sockaddr *) &server, sizeof (struct sockaddr)) == -1)
    {
        perror ("[server]Eroare la bind().\n");
        return errno;
    }

    /*ne conectam la baza de date*/

    if ( connectToDatabase() )
        qDebug()<<"Conexiunea cu baza de date LearNet.db a fost realizata cu succes!";
    else
        qDebug()<<"Eroare la conexiunea cu baza de date."<<db.lastError();

    if ( db.open() )
        qDebug()<<"Baza de date LearNet.db a fost deschisa cu succes!";
    else
        qDebug()<<"Eroare la deschiderea bazei de date."<<db.lastError();

    /* punem serverul sa asculte daca vin clienti sa se conecteze */
    if (listen (sd, 2) == -1)
    {
        perror ("[server]Eroare la listen().\n");
        return errno;
    }
    /* servim in mod concurent clientii...folosind thread-uri */
    while (1)
    {
        int client;
        thData * td; //parametru functia executata de thread
        socklen_t length = sizeof (from);

        printf ("[server]Asteptam la portul %d...\n",PORT);
        fflush (stdout);

        //client= malloc(sizeof(int));
        /* acceptam un client (stare blocanta pina la realizarea conexiunii) */
        if ( (client = accept (sd, (struct sockaddr *) &from, &length)) < 0)
        {
            perror ("[server]Eroare la accept().\n");
            continue;
        }

        /* s-a realizat conexiunea, se astepta mesajul */

        int idThread; //id-ul threadului
        int cl; //descriptorul intors de accept

        td=(struct thData*)malloc(sizeof(struct thData));
        td->idThread=i++;
        td->cl=client;

        pthread_create(&th[i], NULL, &treat, td);

    }//while
    db.close();//inchidem conexiunea cu baza de date
};

static void *treat(void * arg)
{
    struct thData tdL;
    tdL= *((struct thData*)arg);
    printf ("[thread]- %d - Asteptam mesajul...\n", tdL.idThread);
    fflush (stdout);
    pthread_detach(pthread_self());
    printf("Hello clientule!\n");
    raspunde((struct thData*)arg);

    /* am terminat cu acest client, inchidem conexiunea */
    close ((intptr_t)arg);
    return(NULL);

};

int LoginClient (struct thData tdL)
{
    int credentialsLen;
    char *credentials = (char*) malloc (62);

    fflush (stdout); //golim buffer-ul succesiv
    if (read (tdL.cl, &credentialsLen, 4) <= 0)
    {
        printf("[Thread %d]\n",tdL.idThread);
        perror ("Eroare la read()-ul lungimii string-ului de credentials de la client.\n");
    }
    //printf("%d\n",credentialsLen);

    fflush (stdout);
    if (read(tdL.cl,credentials,credentialsLen) <=0)
    {
        printf("[Thread %d]\n",tdL.idThread);
        perror ("Eroare la read()-ul stringului de credentials de la client.\n");
    }
    //printf("%s\n",credentials);

    char* password = (char*) malloc (30);
    char *pch;

    bzero(username, 30);
    bzero(password, 30);

    pch = strtok(credentials, "$");
    strcpy(username, pch);
    pch = strtok(NULL, "$");
    strcpy(password, pch);

    //criptam parola cu functia MD5
    std::string encryptedPassword;
    encryptedPassword=md5(password);

    printf("Cautam in LearNet.db datele de logare si trimitem clientului confirmarea daca acestea sunt gasite.\n");
    int confirmation=0;
    QSqlQuery query;

    char * enc_pass = new char[encryptedPassword.length() + 1];
    std::copy(encryptedPassword.begin(), encryptedPassword.end(), enc_pass);
    enc_pass[encryptedPassword.size()] = '\0';

    /*
    strcpy (interogare, "SELECT * FROM users WHERE username LIKE '");
    strcat (interogare, username);
    strcat (interogare, "%' ;");
    */

    query.prepare("SELECT * FROM users WHERE username=? and encrypted_password=?");
    query.bindValue(0, username);
    query.bindValue(1,enc_pass);

    //executam interogarea si trimitem la server confirmarea
    query.exec();
    if (query.first()) //arata prima inregistrare din tabela care corespunde selectului
    {
        confirmation=1;//am gasit userul
        printf("Am gasit username-ul %s, cu parola criptata %s.\n", username, enc_pass);

        if (write (tdL.cl, &confirmation, 4) <=0)
        {
            printf("[Thread %d] ",tdL.idThread);
            perror ("[Thread]Eroare la write()-ul confirmarii de Login catre client.\n");
        }
    }
    else
    {
        confirmation=0; //username-ul introdus sau parola sunt gresite
        printf("Nu am gasit username-ul %s, cu parola criptata %s.\n", username, enc_pass);
        if (write (tdL.cl, &confirmation, 4) <=0)
        {
            printf("[Thread %d] ",tdL.idThread);
            perror ("[Thread]Eroare la write()-ul confirmarii de Login catre client.\n");
        }
    }
    return confirmation;
}

int RegisterClient (struct thData tdL)
{

    int credentialsLen;
    char *credentials = (char*) malloc (62);

    fflush (stdout); //golim buffer-ul succesiv
    if (read (tdL.cl, &credentialsLen, 4) <= 0)
    {
        printf("[Thread %d]\n",tdL.idThread);
        perror ("Eroare la read()-ul lungimii string-ului de credentials de la client.\n");
    }
    // printf("%d\n",credentialsLen);

    fflush (stdout);
    if (read(tdL.cl,credentials,credentialsLen) <=0)
    {
        printf("[Thread %d]\n",tdL.idThread);
        perror ("Eroare la read()-ul stringului de credentials de la client.\n");
    }
    //printf("%s\n",credentials);

    char* password = (char*) malloc (30);
    char *pch;

    bzero(username, 30);
    bzero(password, 30);

    pch = strtok(credentials, "$");
    strcpy(username, pch);
    pch = strtok(NULL, "$");
    strcpy(password, pch);

    //criptam parola cu functia MD5
    std::string encryptedPassword;
    encryptedPassword=md5(password);

    //std::cout<<username<<" ";

    int confirmation=0;
    QSqlQuery query;

    printf("Cautam in LearNet.db datele de sign up si trimitem clientului confirmarea daca username-ule ste deja existent in baza de date.\n");

    char * enc_pass = new char[encryptedPassword.length() + 1];
    std::copy(encryptedPassword.begin(), encryptedPassword.end(), enc_pass);
    enc_pass[encryptedPassword.size()] = '\0';

    query.prepare("SELECT * FROM users WHERE username=?");
    query.bindValue(0, username);

    //executam interogarea si trimitem la server confirmarea
    query.exec();
    if (query.first()) //arata prima inregistrare din tabela care corespunde selectului
    {
        confirmation=0;//username-ul este deja existent in baza de date

        printf("Username-ul %s este deja existent in baza de date.\n", username);

        if (write (tdL.cl, &confirmation, 4) <=0)
        {
            printf("[Thread %d] ",tdL.idThread);
            perror ("[Thread]Eroare la write()-ul confirmarii de SignUp catre client.\n");
        }
    }
    else
    {
        confirmation=1;

        query.prepare("INSERT INTO users VALUES (NULL, ?, ?)");
        query.bindValue(0,username);
        query.bindValue(1,enc_pass);
        query.exec();

        printf("Am inserat in baza de date username-ul %s, si parola criptata %s.\n", username, enc_pass);

        if (write (tdL.cl, &confirmation, 4) <=0)
        {
            printf("[Thread %d] ",tdL.idThread);
            perror ("[Thread]Eroare la write()-ul confirmarii de SignUp catre client.\n");
        }
    }
    return confirmation;
}

int searchTopicCorrect(struct thData tdL, char optiuneTopic[])
{
    bool confirmare=0; //flag pentru corectitudinea numarului topicului
    int cifra1, cifra2, numberOfTopic;

    if((optiuneTopic[0]!='1') && (optiuneTopic[0]!='2') && (optiuneTopic[0]!='3') && (optiuneTopic[0]!='4') && (optiuneTopic[0]!='5')
            && (optiuneTopic[0]!='6') && (optiuneTopic[0]!='7') && (optiuneTopic[0]!='8') && (optiuneTopic[0]!='9')){
        confirmare=0;
        printf("Serverul infirma corectitudinea numarului de topic.\n");
        return confirmare;
    }
    else{
        cifra1 = optiuneTopic[0] - '0';
        numberOfTopic = cifra1;
    }

    if(optiuneTopic[1]!='\0')
        if((optiuneTopic[1]!='0') && (optiuneTopic[1]!='1')  && (optiuneTopic[1]!='2')  && (optiuneTopic[1]!='3')){
            confirmare=0;
            printf("Serverul infirma corectitudinea numarului de topic.\n");
            return confirmare;
        }
        else{
            cifra2 = optiuneTopic[1] - '0';
            numberOfTopic = numberOfTopic * 10 + cifra2;
            if(numberOfTopic>=14){
                confirmare=0;
                printf("Serverul infirma corectitudinea numarului de topic.\n");
                return confirmare;
            }
            if((numberOfTopic>0) && (numberOfTopic<=13)){
                confirmare=1;
                printf("Serverul confirma corectitudinea numarului de topic.\n");
                return confirmare;
            }
        }

    if((numberOfTopic>0) && (numberOfTopic<=13)){
        confirmare=1;
        printf("Serverul confirma corectitudinea numarului de topic.\n");
        return confirmare;
    }
    return confirmare;
}

void searchInformation(struct thData tdL, char optiuneTopic[])
{
    QSqlQuery query;
    query.prepare("SELECT information FROM technical_infos WHERE id=?");
    query.bindValue(0, optiuneTopic);

    query.exec();//cautam optiunea de topic in baza de date pentru a afisa informatiile

    if (query.next())
    {
        const char* charInformation;

        QString info = query.value("information").toString();
        std::string strInfo=info.toStdString();
        charInformation=strInfo.c_str();
        //printf("%s\n",charInformation);
        //printf("%d\n",strInfo.size());

        int infoLen=strInfo.length();

        //trimitem la client lungimea informatiei
        if (write(tdL.cl,&infoLen,4) <=0)
        {
            printf("[Thread %d] ",tdL.idThread);
            perror ("[Thread]Eroare la write()-ul lungimii informatiei pentru searchWindow catre client.\n");
        }

        //trimitem toata informatia la client
        if (write(tdL.cl,charInformation,strInfo.length()) <=0)
        {
            printf("[Thread %d] ",tdL.idThread);
            perror ("[Thread]Eroare la write()-ul informatiei pentru searchWindow catre client.\n");
        }
    }
}

void searchMessages(struct thData tdL, char optiuneTopic[])
{
    printf("Topicul ales este: %s\n", optiuneTopic);
    char* strTopic=(char*)malloc(30);
    //stabilim topicul
    if(strcmp(optiuneTopic, "1")==0)
        strcpy(strTopic,"generalInfos");
    else if(strcmp(optiuneTopic, "2")==0)
        strcpy(strTopic,"clasificare");
    else if(strcmp(optiuneTopic, "3")==0)
        strcpy(strTopic,"tipuri");
    else if(strcmp(optiuneTopic, "4")==0)
        strcpy(strTopic,"internetworking");
    else if(strcmp(optiuneTopic, "5")==0)
        strcpy(strTopic,"echipamente");
    else if(strcmp(optiuneTopic, "6")==0)
        strcpy(strTopic,"socket");
    else if(strcmp(optiuneTopic, "7")==0)
        strcpy(strTopic,"pipe");
    else if(strcmp(optiuneTopic, "8")==0)
        strcpy(strTopic,"fifo");
    else if(strcmp(optiuneTopic, "9")==0)
        strcpy(strTopic,"istoric");
    else if(strcmp(optiuneTopic, "10")==0)
        strcpy(strTopic,"tcp");
    else if(strcmp(optiuneTopic, "11")==0)
        strcpy(strTopic,"internetProtocol");
    else if(strcmp(optiuneTopic, "12")==0)
        strcpy(strTopic,"adrese");
    else if(strcmp(optiuneTopic, "13")==0)
        strcpy(strTopic,"fisiere");
    printf("Topicul ales este: %s\n", strTopic);

    QSqlQuery query;
    query.prepare("SELECT username FROM forum WHERE topic=?");
    query.bindValue(0, strTopic);

    query.exec();//cautam optiunea de topic in baza de date pentru a afisa informatiile

    //trimitem la client numarul de mesaje aferente topicului respectiv;
    int numberRec=0;
    while (query.next()){
        numberRec++;
    }

    if (write(tdL.cl,&numberRec,4) <=0)
    {
        printf("[Thread %d] ",tdL.idThread);
        perror ("[Thread]Eroare la write()-ul numarul de inregistrari catre client.\n");
    }

    //parcurgem numarul de inregistrari convenabile din baza de date
    for(int i=0; i<numberRec; i++)
    {
        int index=0;

        query.prepare("SELECT username FROM forum WHERE topic=?");
        query.bindValue(0, strTopic);
        query.exec();
        //parcurgem baza de date pana la inregistrarea curenta si selectam mai intai numele, dupa mesajul
        while ((query.next())&&(index!=i))
        {
            index++;
        }
        const char* charNume;

        QString nume = query.value("username").toString();
        std::string strNume=nume.toStdString();
        charNume=strNume.c_str();

        int numeLen=strNume.length();

        //trimitem la client lungimea username-ului
        if (write(tdL.cl,&numeLen,4) <=0)
        {
            printf("[Thread %d] ",tdL.idThread);
            perror ("[Thread]Eroare la write()-ul lungimii username-ului aferent unui mesaj pentru Forum catre client.\n");
        }

        //trimitem la client username-ul care a scris un mesaj pe forum
        if (write(tdL.cl,charNume,strNume.length()) <=0)
        {
            printf("[Thread %d] ",tdL.idThread);
            perror ("[Thread]Eroare la write()-ul username-ului aferent unui mesaj pentru Forum catre client.\n");
        }
        index=0;

        query.prepare("SELECT message FROM forum WHERE topic=?");
        query.bindValue(0, strTopic);
        query.exec();
        //parcurgem baza de date pana la inregistrarea curenta si selectam mai intai numele, dupa mesaju;
        while ((query.next())&&(index!=i))
        {
            index++;
        }
        const char* charMessage;

        QString mess = query.value("message").toString();
        std::string strMess=mess.toStdString();
        charMessage=strMess.c_str();

        int messLen=strMess.length();

        //trimitem la client lungimea mesajului
        if (write(tdL.cl,&messLen,4) <=0)
        {
            printf("[Thread %d] ",tdL.idThread);
            perror ("[Thread]Eroare la write()-ul lungimii mesajului pentru Forum catre client.\n");
        }

        //trimitem mesajul propriu-zis la client
        if (write(tdL.cl,charMessage,messLen) <=0)
        {
            printf("[Thread %d] ",tdL.idThread);
            perror ("[Thread]Eroare la write()-ul mesajului pentru Forum catre client.\n");
        }
    }
}

void addMessage(struct thData tdL, char optiuneTopic[], char nume[])
{
    char* strTopic=(char*)malloc(30);
    //stabilim topicul
    if(strcmp(optiuneTopic, "1")==0)
        strcpy(strTopic,"generalInfos");
    else if(strcmp(optiuneTopic, "2")==0)
        strcpy(strTopic,"clasificare");
    else if(strcmp(optiuneTopic, "3")==0)
        strcpy(strTopic,"tipuri");
    else if(strcmp(optiuneTopic, "4")==0)
        strcpy(strTopic,"internetworking");
    else if(strcmp(optiuneTopic, "5")==0)
        strcpy(strTopic,"echipamente");
    else if(strcmp(optiuneTopic, "6")==0)
        strcpy(strTopic,"socket");
    else if(strcmp(optiuneTopic, "7")==0)
        strcpy(strTopic,"pipe");
    else if(strcmp(optiuneTopic, "8")==0)
        strcpy(strTopic,"fifo");
    else if(strcmp(optiuneTopic, "9")==0)
        strcpy(strTopic,"istoric");
    else if(strcmp(optiuneTopic, "10")==0)
        strcpy(strTopic,"tcp");
    else if(strcmp(optiuneTopic, "11")==0)
        strcpy(strTopic,"internetProtocol");
    else if(strcmp(optiuneTopic, "12")==0)
        strcpy(strTopic,"adrese");
    else if(strcmp(optiuneTopic, "13")==0)
        strcpy(strTopic,"fisiere");
    printf("Adaugam mesaje la topicul: %s\n", strTopic);

    int mesajLen=0;
    char* mesaj=(char*)malloc(300);

    fflush(stdout);
    //citim lungimea mesajului primit de la server
    if (read (tdL.cl, &mesajLen,4) < 0) {
        qDebug()<<"[client]Eroare la write()-ul lungimii mesajului (Forum) spre server.\n";
    }
    printf("%d\n", mesajLen);
    fflush(stdout);
    //citim mesajul de la server
    if (read (tdL.cl, mesaj,mesajLen) < 0) {
        qDebug()<<"[client]Eroare la write()-ul mesajului (Forum) spre server.\n";
    }
    mesaj[mesajLen]='\0';
    printf("%s\n", mesaj);

    QSqlQuery query;
    query.prepare("INSERT INTO forum VALUES (?, ?, ?);");
    query.bindValue(0, nume);
    query.bindValue(1, strTopic);
    query.bindValue(2, mesaj);

    query.exec();

    int confirmInsert=1;

    //trimitem la client confirmarea insert-ului
    if (write(tdL.cl,&confirmInsert,4) <=0)
    {
        printf("[Thread %d] ",tdL.idThread);
        perror ("[Thread]Eroare la write()-ul confirmarii insert-ului pentru forum catre client.\n");
    }
    printf("Am inserat in tabela Forum, mesajul %s de la username-ul %s, pentru topicul %s.\n", mesaj, nume, strTopic);
}

void showListOfUsers(struct thData tdL,char name[])
{
    QSqlQuery query;
    query.prepare("SELECT username FROM users where username<>?");
    query.bindValue(0, name);
    query.exec();

    //trimitem la client numarul de utilizatori;
    int numberRec=0;
    while (query.next()){
        numberRec++;
    }

    if (write(tdL.cl,&numberRec,4) <=0)
    {
        printf("[Thread %d] ",tdL.idThread);
        perror ("[Thread]Eroare la write()-ul numarul de inregistrari catre client.\n");
    }

    //parcurgem numarul de inregistrari din baza de date
    for(int i=0; i<numberRec; i++)
    {
        int index=0;

        query.prepare("SELECT username FROM users where username<>?");
        query.bindValue(0, name);
        query.exec();

        //parcurgem baza de date pana la inregistrarea curenta si selectam numele
        while ((query.next())&&(index!=i))
        {
            index++;
        }
        const char* charNume;

        QString nume = query.value("username").toString();
        std::string strNume=nume.toStdString();
        charNume=strNume.c_str();

        int numeLen=strNume.length();

        //trimitem la client lungimea username-ului
        if (write(tdL.cl,&numeLen,4) <=0)
        {
            printf("[Thread %d] ",tdL.idThread);
            perror ("[Thread]Eroare la write()-ul lungimii username-ului pentru lista de useri catre client.\n");
        }

        //trimitem la client username-ul
        if (write(tdL.cl,charNume,numeLen) <=0)
        {
            printf("[Thread %d] ",tdL.idThread);
            perror ("[Thread]Eroare la write()-ul username-ului pentru lista de useri catre client.\n");
        }
    }
}

void showListOfFriends(struct thData tdL,char name[])//facem in asa fel incat relatia de prietenie sa fie una mutuala
{
    //vedem mai intai in care inregistrari user-ul curent se afla in dreapta
    QSqlQuery query;
    query.prepare("SELECT u2.username FROM users u1 JOIN friends f ON u1.id = f.id1 JOIN users u2 ON u2.id = f.id2 WHERE u1.username = ?");
    query.bindValue(0, name);
    query.exec();


    int numberRec=0, numberRec1=0, numberRec2=0;
    while (query.next()){
        numberRec1++;
    }

    //vedem in cate inregistrari userul curent se afla in stanga (in asa fel realizat prietenia mutuala)
    QSqlQuery query1;
    query1.prepare("SELECT u1.username FROM users u1 JOIN friends f ON u1.id = f.id1 JOIN users u2 ON u2.id = f.id2 WHERE u2.username = ?");
    query1.bindValue(0, name);
    query1.exec();

    while (query1.next()){
        numberRec2++;
    }

    numberRec = numberRec1 + numberRec2;

    //trimitem la client numarul de prieteni;
    if (write(tdL.cl,&numberRec,4) <=0)
    {
        printf("[Thread %d] ",tdL.idThread);
        perror ("[Thread]Eroare la write()-ul numarul de inregistrari catre client.\n");
    }

    //parcurgem numarul de inregistrari din baza de date in care utilizatorul se afla in partea stanga
    for(int i=0; i<numberRec1; i++)
    {
        int index=0;

        query.prepare("SELECT u2.username FROM users u1 JOIN friends f ON u1.id = f.id1 JOIN users u2 ON u2.id = f.id2 WHERE u1.username = ?");
        query.bindValue(0, name);
        query.exec();

        //parcurgem baza de date pana la inregistrarea curenta si selectam numele
        while ((query.next())&&(index!=i))
        {
            index++;
        }
        const char* charNume;

        QString nume = query.value("username").toString();
        std::string strNume=nume.toStdString();
        charNume=strNume.c_str();

        int numeLen=strNume.length();

        //trimitem la client lungimea username-ului
        if (write(tdL.cl,&numeLen,4) <=0)
        {
            printf("[Thread %d] ",tdL.idThread);
            perror ("[Thread]Eroare la write()-ul lungimii username-ului pentru lista de prieteni catre client.\n");
        }

        //trimitem la client username-ul
        if (write(tdL.cl,charNume,numeLen) <=0)
        {
            printf("[Thread %d] ",tdL.idThread);
            perror ("[Thread]Eroare la write()-ul username-ului pentru lista de prieteni catre client.\n");
        }
    }
    //parcurgem numarul de inregistrari din baza de date in care utilizatorul se afla in partea dreapta
    for(int i=0; i<numberRec2; i++)
    {
        int index=0;

        query1.prepare("SELECT u1.username FROM users u1 JOIN friends f ON u1.id = f.id1 JOIN users u2 ON u2.id = f.id2 WHERE u2.username = ?");
        query1.bindValue(0, name);
        query1.exec();

        //parcurgem baza de date pana la inregistrarea curenta si selectam numele
        while ((query1.next())&&(index!=i))
        {
            index++;
        }
        const char* charNume;

        QString nume = query1.value("username").toString();
        std::string strNume=nume.toStdString();
        charNume=strNume.c_str();

        int numeLen=strNume.length();

        //trimitem la client lungimea username-ului
        if (write(tdL.cl,&numeLen,4) <=0)
        {
            printf("[Thread %d] ",tdL.idThread);
            perror ("[Thread]Eroare la write()-ul lungimii username-ului pentru lista de prieteni catre client.\n");
        }

        //trimitem la client username-ul
        if (write(tdL.cl,charNume,numeLen) <=0)
        {
            printf("[Thread %d] ",tdL.idThread);
            perror ("[Thread]Eroare la write()-ul username-ului pentru lista de prieteni catre client.\n");
        }
    }
}


int addFriend(struct thData tdL,char name[])
{
    int faVerifInServer;
    int confirmation=0;

    //vedem daca se face sau nu verificarea in server;
    fflush(stdout);
    if (read (tdL.cl,&faVerifInServer ,4) <= 0)
    {
        printf("[Thread %d]\n",tdL.idThread);
        perror ("Eroare la read() nume username (Friends Window) de la client.\n");
    }

    int friendLen;
    char* charFriend=(char*)malloc(30);

    if(faVerifInServer==1){
        fflush(stdout);
        if (read (tdL.cl,&friendLen,4) <= 0)
        {
            printf("[Thread %d]\n",tdL.idThread);
            perror ("Eroare la read() lungime username (Friends Window) de la client.\n");
        }
        fflush(stdout);
        if (read (tdL.cl,charFriend, friendLen) <= 0)
        {
            printf("[Thread %d]\n",tdL.idThread);
            perror ("Eroare la read()  username (Friends Window) de la client.\n");
        }
        fflush(stdout);
        charFriend[friendLen]='\0';
        printf("Serverul a primit username-ul pentru Friends Window: %s\n", charFriend);

        if(strcmp(charFriend, name)==0)
        {
            confirmation=2; //username-ul introdus este fix username-ul clientului
            printf("Am gasit username-ul %s.\n", charFriend);
            if (write (tdL.cl, &confirmation, 4) <=0)
            {
                printf("[Thread %d] ",tdL.idThread);
                perror ("[Thread]Eroare la write()-ul confirmarii de user pentru friends window catre client.\n");
            }
            return confirmation;
        }
        else{

            QSqlQuery query;
            query.prepare("SELECT * FROM users WHERE username=?");
            query.bindValue(0, charFriend);

            //executam interogarea si trimitem la server confirmarea
            query.exec();
            if (query.first()) //arata prima inregistrare din tabela care corespunde selectului
            {
                QSqlQuery query2;
                query2.prepare("SELECT u2.username FROM users u1 JOIN friends f ON u1.id = f.id1 JOIN users u2 ON u2.id = f.id2 WHERE u1.username = ?");
                query2.bindValue(0, name);
                query2.exec();

                //vedem daca userul nu este cumva deja in lista de prieteni
                while(query2.next())
                {
                    const char* charNume;
                    QString nume = query2.value("username").toString();
                    std::string strNume=nume.toStdString();
                    charNume=strNume.c_str();

                    if(strcmp(charFriend,charNume)==0)
                    {
                        confirmation=3;//username-ul care a fost introdus se afla deja in lista de prieteni
                        if (write (tdL.cl, &confirmation, 4) <=0)
                        {
                            printf("[Thread %d] ",tdL.idThread);
                            perror ("[Thread]Eroare la write()-ul confirmarii de user pentru friends window catre client.\n");
                        }
                        return confirmation;
                    }
                }
                QSqlQuery query3;
                query3.prepare("SELECT u1.username FROM users u1 JOIN friends f ON u1.id = f.id1 JOIN users u2 ON u2.id = f.id2 WHERE u2.username = ?");
                query3.bindValue(0, name);
                query3.exec();

                //vedem daca userul nu este cumva deja in lista de prieteni
                while(query3.next())
                {
                    const char* charNume;
                    QString nume = query3.value("username").toString();
                    std::string strNume=nume.toStdString();
                    charNume=strNume.c_str();

                    if(strcmp(charFriend,charNume)==0)
                    {
                        confirmation=3;//username-ul care a fost introdus se afla deja in lista de prieteni
                        if (write (tdL.cl, &confirmation, 4) <=0)
                        {
                            printf("[Thread %d] ",tdL.idThread);
                            perror ("[Thread]Eroare la write()-ul confirmarii de user pentru friends window catre client.\n");
                        }
                        return confirmation;
                    }
                }
                confirmation=1;//am gasit userul
                printf("Am gasit username-ul %s.\n", charFriend);

                //inseram in tabela friends noua pereche de id-uri deoarece trimiterea cererii de prietenie am vazut ca este posibila
                QSqlQuery query4, query5, query6;
                int nrID1, nrID2;

                //selectam mai intai id-ul username-ului curent
                query5.prepare("select id from users where username=?");
                query5.bindValue(0, name);
                query5.exec();

                if(query5.first())
                {
                    QString id1 = query5.value("id").toString();
                    std::string strID1=id1.toStdString();
                    nrID1=atoi(strID1.c_str());
                }

                //selectam apoi id-ul prietenului caruia dorim sa ii trimitem cererea de prietenie
                query6.prepare("select id from users where username=?");
                query6.bindValue(0, charFriend);
                query6.exec();

                if(query6.first())
                {
                    QString id2 = query6.value("id").toString();
                    std::string strID2=id2.toStdString();
                    nrID2=atoi(strID2.c_str());
                }

                query4.prepare("INSERT INTO friends VALUES (?, ?)");
                query4.bindValue(0, nrID1);
                query4.bindValue(1, nrID2);
                query4.exec();

                if (write (tdL.cl, &confirmation, 4) <=0)
                {
                    printf("[Thread %d] ",tdL.idThread);
                    perror ("[Thread]Eroare la write()-ul confirmarii de user pentru friends window catre client.\n");
                }
                printf("Am inserat in tabela Friends, o noua cerere de prietenie, de la userul %s la userul %s.\n", name, charFriend);
                return confirmation;
            }
            else
            {
                confirmation=0; //username-ul introdus este gresit
                printf("Nu am gasit username-ul %s.\n", charFriend);
                if (write (tdL.cl, &confirmation, 4) <=0)
                {
                    printf("[Thread %d] ",tdL.idThread);
                    perror ("[Thread]Eroare la write()-ul confirmarii de user pentru friends window catre client.\n");
                }
                return confirmation;
            }
        }
    }
    return confirmation;
}


int keywordsSearch (struct thData tdL)
{
    int keywordLen;
    char* keyword=(char*)malloc(30);

    //citim de la client mai intai lungimea keyword-ului si dupa keyword-ul propriu zis
    if (read (tdL.cl,&keywordLen ,4) <= 0)
    {
        printf("[Thread %d]\n",tdL.idThread);
        perror ("Eroare la read() lungime keyword de la client.\n");
    }

    if (read (tdL.cl,keyword,keywordLen) <= 0)
    {
        printf("[Thread %d]\n",tdL.idThread);
        perror ("Eroare la read() keyword de la client.\n");
    }

    keyword[keywordLen]='\0';
    QSqlQuery query1;
    query1.prepare("SELECT information FROM technical_infos");
    query1.exec();
    int nrTopics=0;

    while (query1.next()){
        const char* charInformation;
        QString info = query1.value("information").toString();
        std::string strInfo=info.toStdString();
        charInformation=strInfo.c_str();

        if(strstr(charInformation,keyword)!=NULL)
            nrTopics++;
    }

    //trimitem la client numarul de topicuri in care apare cuvantul
    if (write(tdL.cl,&nrTopics,4) <=0)
    {
        printf("[Thread %d] ",tdL.idThread);
        perror ("[Thread]Eroare la write()-ul numarul de topicuri in care apare keyword-ul catre client.\n");
    }

    QSqlQuery query;
    query.prepare("SELECT information FROM technical_infos");
    query.exec();//selectam toata informatia din baza de date

    while (query.next())
    {
        const char* charInformation;

        QString info = query.value("information").toString();
        std::string strInfo=info.toStdString();
        charInformation=strInfo.c_str();

        int infoLen=strInfo.length();
        char* aux=(char*)malloc(1350);
        strcpy(aux,charInformation);

        char* p = strstr(aux, keyword);
        int occurrences=0;
        bool found=0;

        //cautam nr total de aparitii ale lui keyword in charInformation
        while(p!=NULL)
        {
            p++;
            occurrences++;
            p=strstr(p,keyword);
        }
        if(occurrences!=0){
            QSqlQuery query2;
            query2.prepare("SELECT topic FROM technical_infos where information=?");
            query2.bindValue(0,charInformation);
            query2.exec();
            if(query2.first()){

                const char* charTopic;

                QString topic = query2.value("topic").toString();
                std::string strTopic=topic.toStdString();
                charTopic=strTopic.c_str();

                int topicLen=strTopic.length();

                //trimitem la client lungimea topicului in care am gasit keyword-ul
                if (write(tdL.cl,&topicLen,4) <=0)
                {
                    printf("[Thread %d] ",tdL.idThread);
                    perror ("[Thread]Eroare la write()-ul lungimii topicului pentru keyword search catre client.\n");
                }

                //trimitem la client numele efectiv al topicului in care am gasit keywordul
                if (write(tdL.cl,charTopic,strTopic.length()) <=0)
                {
                    printf("[Thread %d] ",tdL.idThread);
                    perror ("[Thread]Eroare la write()-ul topicului pentru keyword search catre client.\n");
                }
            }

            if (write(tdL.cl,&occurrences,4) <=0)
            {
                printf("[Thread %d] ",tdL.idThread);
                perror ("[Thread]Eroare la write()-ul numarului de aparitii ale keywordului catre client.\n");
            }
        }
    }
}


void raspunde(void *arg)
{
    struct thData tdL;
    tdL= *((struct thData*)arg);
    bool reconnect=1;

    while(reconnect==1){
        int optiuneLoginSignUp=0;
        bool nelogat=1;
        int aux;

        while(nelogat==1)
        {
            //verificam, mai intai daca clientul doreste sa se loggeze sau daca vrea sa se inregistreze
            if (read (tdL.cl, &optiuneLoginSignUp,4) <= 0)
            {
                printf("[Thread %d]\n",tdL.idThread);
                perror ("Eroare la read() optiune LoginOrSignUp de la client.\n");
            }
            if (optiuneLoginSignUp == 1)
            {
                aux=LoginClient(tdL);
                if(aux==1)
                    nelogat=0;
            }
            else
            {
                aux=RegisterClient(tdL);
                if(aux==1)
                    nelogat=0;
            }
            //facem register sau login pentru fiecare client in parte, de aceea avem nevoie de parametrul tdL
        }

        // ********SEARCH WINDOW/ FORUM/ FRIENDS ***********

        int whatClientChose;
        int signOut=0;
        while(signOut==0) //facem asta cat timp clientul are aplicatia deschisa si nu doreste sa se deconecteze
        {

            int topicCorect=0;
            char optiuneTopic[2];
            int auxilary;
            int faVerifInServer;

            //citim ce vrea utilizatorul sa faca
            if (read (tdL.cl,&whatClientChose ,4) <= 0)
            {
                printf("[Thread %d]\n",tdL.idThread);
                perror ("Eroare la read() optiune de topic de la client.\n");
            }

            if(whatClientChose==1) {
                printf("Clientul a ales sa faca Search de informatii despre retele.\n");

                //vedem daca se face sau nu verificarea in server;
                if (read (tdL.cl,&faVerifInServer ,4) <= 0)
                {
                    printf("[Thread %d]\n",tdL.idThread);
                    perror ("Eroare la read() optiune de topic de la client.\n");
                }

                if(faVerifInServer==1){
                    if (read (tdL.cl,optiuneTopic ,2) <= 0)
                    {
                        printf("[Thread %d]\n",tdL.idThread);
                        perror ("Eroare la read() optiune de topic de la client.\n");
                    }
                    printf("Serverul a primit optiunea de topic pentru Search Window: %s\n", optiuneTopic);

                    auxilary=searchTopicCorrect(tdL, optiuneTopic);

                    if(auxilary==1){
                        topicCorect=1;
                    }

                    fflush(stdout);

                    if (write (tdL.cl, &auxilary, 4) <=0)
                    {
                        printf("[Thread %d] ",tdL.idThread);
                        perror ("[Thread]Eroare la write()-ul confirmarii numarului de topic catre client.\n");
                    }
                    searchInformation(tdL, optiuneTopic);
                }
            }
            else if(whatClientChose==2)
            {
                printf("Clientul a ales sa vada mesajele de pe forum aferente unui anumit topic despre retele.\n");

                //vedem daca se face sau nu verificarea in server;
                if (read (tdL.cl,&faVerifInServer ,4) <= 0)
                {
                    printf("[Thread %d]\n",tdL.idThread);
                    perror ("Eroare la read() optiune de topic de la client.\n");
                }

                if(faVerifInServer==1){
                    if (read (tdL.cl,optiuneTopic ,2) <= 0)
                    {
                        printf("[Thread %d]\n",tdL.idThread);
                        perror ("Eroare la read() optiune de topic de la client.\n");
                    }
                    printf("Serverul a primit optiunea de topic pentru forum: %s\n", optiuneTopic);

                    auxilary=searchTopicCorrect(tdL, optiuneTopic);

                    if(auxilary==1){
                        topicCorect=1;
                    }

                    fflush(stdout);

                    if (write (tdL.cl, &auxilary, 4) <=0)
                    {
                        printf("[Thread %d] ",tdL.idThread);
                        perror ("[Thread]Eroare la write()-ul confirmarii numarului de topic catre client.\n");
                    }
                    searchMessages(tdL, optiuneTopic);
                }
            }
            else if(whatClientChose==3)
            {
                printf("Clientul a ales sa trimita un mesaj.\n");

                //citim optiunea de topic
                if (read (tdL.cl,optiuneTopic ,2) <= 0)
                {
                    printf("[Thread %d]\n",tdL.idThread);
                    perror ("Eroare la read() optiune de topic de la client.\n");
                }
                addMessage(tdL, optiuneTopic, username);
                //sa trimit confirmare inapoi la client
            }
            else if(whatClientChose==4)
            {
                printf("Clientul a ales sa vada lista de utilizatori.\n");
                showListOfUsers(tdL, username);
            }
            else if(whatClientChose==5)
            {
                printf("Clientul a ales sa vada lista de prieteni.\n");
                showListOfFriends(tdL, username);
            }
            else if(whatClientChose==6)
            {
                printf("Clientul doreste sa trimita o cerere de prietenie.\n");
                addFriend(tdL, username);
            }
            else if(whatClientChose==7)
            {
                printf("Clientul doreste sa se deconecteze si sa se loggeze pe un alt cont.\n");
                signOut=1;
            }
            else if(whatClientChose==8)
            {
                printf("Clientul a ales sa inchida aplicatia.\n");
                reconnect=0;
            }
            else if(whatClientChose==9)
            {
                printf("Clientul a ales Keywords Search.\n");
                keywordsSearch(tdL);
            }
        }
    }
}

