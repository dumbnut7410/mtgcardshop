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

INCLUDEPATH+= /opt/Qt/Tools/QtCreator/bin/plugins/sqldrivers/libqsqlite.so
#INCLUDEPATH += /usr/local/qt5pi/include/

LIBS += /opt/Qt/Tools/QtCreator/bin/plugins/sqldrivers/libqsqlite.so
#LIBS += /usr/local/qt5pi/plugins/sqldrivers/libqsqlite.so

SOURCES += main.cpp \
    sqlwriter.cpp

HEADERS += \
    sqlwriter.h \
    player.h \
    inventoryItem.h \
    transaction.h
