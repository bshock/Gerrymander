#-------------------------------------------------
#
# Project created by QtCreator 2016-05-12T22:00:41
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = gerrymander
TEMPLATE = app


SOURCES += main.cpp \
    boardwidget.cpp \
    district.cpp \
    electionmap.cpp \
    calendar.cpp

HEADERS  += \
    boardwidget.h \
    district.h \
    piecepasser.h \
    electionmap.h \
    calendar.h \
    timecontrol.h \
    turnchange.h

FORMS    +=

RESOURCES += \
    gerrymander.qrc

DISTFILES += \
    words.txt
