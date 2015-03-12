#-------------------------------------------------
#
# Project created by QtCreator 2014-09-22T21:25:01
#
#-------------------------------------------------

QT       += core network

QT       -= gui

TARGET = DroneFind
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


HEADERS += $$PWD/../Shared/*.h
SOURCES += $$PWD/../Shared/*.cpp
HEADERS += *.h
SOURCES += *.cpp


CONFIG += c++11

# Version
GIT_VERSION = $$system(git --git-dir $$PWD/../.git --work-tree $$PWD/.. describe --always --tags --long) #  | sed \'s|-|.|g\'
GIT_VERSION = $$replace(GIT_VERSION, -, .)
message(GIT_VERSION: $$GIT_VERSION)
DEFINES += GIT_VERSION=\\\"$$GIT_VERSION\\\"
