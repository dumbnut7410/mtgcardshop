#-------------------------------------------------
#
# Project created by QtCreator 2015-02-27T19:49:49
#
#-------------------------------------------------

QT       += core
QT       += sql
QT       -= gui

TARGET = tmp
CONFIG   += console
CONFIG   -= app_bundle
CONFIG   += C++11
TEMPLATE = app

INCLUDEPATH+= /opt/Qt/Tools/QtCreator/bin/plugins/sqldrivers/libqsqlmysql.so

LIBS+=/opt/Qt/Tools/QtCreator/bin/plugins/sqldrivers/libqsqlmysql.so

SOURCES += main.cpp \
    sqlwriter.cpp

HEADERS += \
    sqlwriter.h \
    player.h \
    inventoryItem.h \
    transaction.h
