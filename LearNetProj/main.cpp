#include "dialog.h"
#include <QApplication>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
int port=2020;
//#include<utils.h>
int sd;
struct sockaddr_in server;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Dialog w;
    w.show();

    int nr=0;
    if((sd = socket(AF_INET, SOCK_STREAM,0)) == -1){

    }
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_port = htons(port);

    if(connect(sd, (struct sockaddr *) &server, sizeof(struct sockaddr)) == -1){
        //error message
    }
    //nr = 5;
    //write(sd, &nr, 4);

    return a.exec();
}


