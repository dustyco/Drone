#-------------------------------------------------
#
# Project created by QtCreator 2014-09-22T21:25:01
#
#-------------------------------------------------

QT       += core network

QT       -= gui

TARGET = DroneTower
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


HEADERS += $$PWD/../Shared/*.h
SOURCES += $$PWD/../Shared/*.cpp
HEADERS += *.h
SOURCES += *.cpp


CONFIG += c++11
