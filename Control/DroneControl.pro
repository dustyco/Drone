TEMPLATE = app

QT += qml quick network widgets


HEADERS += $$PWD/../Shared/*.h
SOURCES += $$PWD/../Shared/*.cpp
HEADERS += *.h
SOURCES += *.cpp

RESOURCES += qml.qrc

CONFIG   += c++11

# Operating system
message($$QMAKESPEC)
win32 {
    message(OS_WINDOWS)
    DEFINES += OS_WINDOWS
    HEADERS += $$PWD/../Shared/OS_Windows/*.h
    SOURCES += $$PWD/../Shared/OS_Windows/*.cpp
    #HEADERS += OS_Windows/*.h
    #SOURCES += OS_Windows/*.cpp
} else: macx {
    message(OS_OSX)
    DEFINES += OS_OSX
    HEADERS += $$PWD/../Shared/OS_OSX/*.h
    SOURCES += $$PWD/../Shared/OS_OSX/*.cpp
    #HEADERS += OS_OSX/*.h
    #SOURCES += OS_OSX/*.cpp
    LIBS += -framework IOKit
} else: unix:!macx {
    message(OS_LINUX)
    DEFINES += OS_LINUX
    HEADERS += $$PWD/../Shared/OS_Linux/*.h
    SOURCES += $$PWD/../Shared/OS_Linux/*.cpp
    #HEADERS += OS_Linux/*.h
    #SOURCES += OS_Linux/*.cpp
    LIBS += -ludev
} else {
    message(OS_GENERIC)
    DEFINES += OS_GENERIC
    #HEADERS += $$PWD/../Shared/OS_Generic/*.h
    #SOURCES += $$PWD/../Shared/OS_Generic/*.cpp
    #HEADERS += OS_Generic/*.h
    #SOURCES += OS_Generic/*.cpp
}

#message(===============================)
#message(Windows spec: $$find(QMAKESPEC, [Ww]in32))
#message(OSX spec: $$find(QMAKESPEC, [Mm]acx))
#message(Linux spec: $$find(QMAKESPEC, [Ll]inux))

# Version
GIT_VERSION = $$system(git --git-dir $$PWD/../.git --work-tree $$PWD/.. describe --always --tags --long) #  | sed \'s|-|.|g\'
GIT_VERSION = $$replace(GIT_VERSION, -, .)
message(GIT_VERSION: $$GIT_VERSION)
DEFINES += GIT_VERSION=\\\"$$GIT_VERSION\\\"

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)
