#-------------------------------------------------
#
# Project created by QtCreator 2013-01-18T10:49:28
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = NCServer
TEMPLATE = app
#win32: LIBS += wsock32.lib

SOURCES += main.cpp\
        serverwindow.cpp \
    waitingserver.cpp \
    connection.cpp \
    serverconfig.cpp \
    protocol/packet.cpp \
    datahandler.cpp \
    dhtcp/dhtcp.cpp \
    dhudp/dhudp.cpp \
    gui/droplineedit.cpp \
    dhtcp/dhtcpencoder.cpp \
    protocol/rawblock.cpp \
    dhudp/dhudpencoder.cpp \
    dhudp/decparams.cpp \
    dhudp/fragment.cpp \
    execthread.cpp

HEADERS  += serverwindow.h \
    waitingserver.h \
    connection.h \
    serverconfig.h \
    protocol/cmd_define.h \
    protocol/protocoltypes.h \
    protocol/packet.h \
    protocol/ports_define.h \
    datahandler.h \
    dhtcp/dhtcp.h \
    dhudp/dhudp.h \
    dhtcp/dhtcpprotocol.h \
    gui/droplineedit.h \
    dhtcp/dhtcpencoder.h \
    protocol/rawblock.h \
    dhudp/dhudpencoder.h \
    dhudp/dhudpprotocol.h \
    dhudp/decparams.h \
    dhudp/fragment.h \
    execthread.h

FORMS    += serverwindow.ui

RESOURCES +=
