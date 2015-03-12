TEMPLATE = app

QT += qml quick network widgets


HEADERS += ../Shared/*.h
SOURCES += ../Shared/*.cpp
HEADERS += *.h
SOURCES += *.cpp

RESOURCES += qml.qrc

# Version
GIT_VERSION = $$system(git --git-dir $$PWD/../.git --work-tree $$PWD/.. describe --always --tags --long) #  | sed \'s|-|.|g\'
GIT_VERSION = $$replace(GIT_VERSION, -, .)
message(GIT_VERSION: $$GIT_VERSION)
DEFINES += GIT_VERSION=\\\"$$GIT_VERSION\\\"

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)
