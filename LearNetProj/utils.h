#include<cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <QDebug>
#include <arpa/inet.h>

#ifndef UTILS_H
#define UTILS_H

#endif // UTILS_H

extern int errno;
extern int sd;
extern struct sockaddr_in server;
extern int loginOrSignUp;
