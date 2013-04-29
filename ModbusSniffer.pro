#-------------------------------------------------
#
# Project created by QtCreator 2013-04-29T20:38:31
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ModbusSniffer
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    rspacket.cpp \
    widgetsender.cpp \
    widgetcom.cpp \
    hled.cpp

HEADERS  += mainwindow.h \
        rs485.h \
    rspacket.h \
    widgetsender.h \
    widgetcom.h \
    hled.h


FORMS    += mainwindow.ui \
    widgetsender.ui \
    widgetcom.ui

INCLUDEPATH += treePages

include (treePages/TrackWidget.pro)
include(qextserialport/src/qextserialport.pri)
