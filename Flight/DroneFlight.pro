#-------------------------------------------------
#
# Project created by QtCreator 2014-09-22T21:25:01
#
#-------------------------------------------------

QT = core network serialport

TARGET = DroneFlight
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


#HEADERS += $$PWD/../Shared/*.h
#SOURCES += $$PWD/../Shared/*.cpp

HEADERS += $$PWD/../Shared/Config.h
SOURCES += $$PWD/../Shared/Config.cpp

HEADERS += $$PWD/../Shared/Discover.h
SOURCES += $$PWD/../Shared/Discover.cpp

HEADERS += $$PWD/../Shared/Identity.h
SOURCES += $$PWD/../Shared/Identity.cpp

HEADERS += $$PWD/../Shared/Logger.h
SOURCES += $$PWD/../Shared/Logger.cpp

HEADERS += $$PWD/../Shared/Messages.h

HEADERS += $$PWD/../Shared/Record.h
SOURCES += $$PWD/../Shared/Record.cpp

HEADERS += *.h
SOURCES += *.cpp


CONFIG   += c++11

# Version
GIT_VERSION = $$system(git --git-dir $$PWD/../.git --work-tree $$PWD/.. describe --always --tags --long) #  | sed \'s|-|.|g\'
GIT_VERSION = $$replace(GIT_VERSION, -, .)
message(GIT_VERSION: $$GIT_VERSION)
DEFINES += GIT_VERSION=\\\"$$GIT_VERSION\\\"
