TEMPLATE = app

QT += qml quick network widgets


HEADERS += $$PWD/../Shared/*.h
SOURCES += $$PWD/../Shared/*.cpp
HEADERS += *.h
SOURCES += *.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

#
CONFIG   += c++11

# Default rules for deployment.
include(deployment.pri)
