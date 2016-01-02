#-------------------------------------------------
#
# Project created by QtCreator 2015-12-30T13:05:18
#
#-------------------------------------------------

QT       += core gui concurrent

QMAKE_CXXFLAGS += -std=c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = charclipper
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    appmenu.cpp \
    ucd.cpp

HEADERS  += mainwindow.h \
    appmenu.h \
    ucd.h

FORMS    += mainwindow.ui
