QT += core network gui widgets


CONFIG += static

TARGET = QTcpServer

CONFIG -= app_bundle


TEMPLATE = app

SOURCES += main.cpp \
    tcpserver.cpp \
    telegram.cpp \
    mainwindow.cpp

HEADERS += \
    tcpserver.h \
    telegram.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

RESOURCES += \
    images.qrc
