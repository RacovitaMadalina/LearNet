#-------------------------------------------------
#
# Project created by QtCreator 2017-12-28T17:20:01
#
#-------------------------------------------------

QT       += core gui
QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LearNetProj
TEMPLATE = app


SOURCES += main.cpp\
    signupwind.cpp \
    searchwindow.cpp \
    forum.cpp \
    friends.cpp \
    dialog.cpp \
    welcomewindow.cpp \
    md5.cpp \
    exit.cpp \
    keywords.cpp

HEADERS  += dialog.h \
    signupwind.h \
    searchwindow.h \
    forum.h \
    friends.h \
    welcomewindow.h \
    utils.h \
    md5.h \
    exit.h \
    keywords.h

FORMS    += dialog.ui \
    signup.ui \
    signupwind.ui \
    searchwindow.ui \
    forum.ui \
    friends.ui \
    welcomewindow.ui \
    exit.ui \
    keywords.ui
