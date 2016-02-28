#-------------------------------------------------
#
# Project created by QtCreator 2015-12-07T21:05:51
#
#-------------------------------------------------

QT       += core gui \
        script \
        scripttools

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CATs
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    highlighter.cpp

HEADERS  += mainwindow.h \
    highlighter.h \
    q_debugstream.h \
    scriptthread.hpp

FORMS    += mainwindow.ui

RESOURCES += \
    resources.qrc
