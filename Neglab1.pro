TEMPLATE = app

QT += qml quick serialport
CONFIG += c++11

SOURCES += main.cpp \
    devicetest.cpp \
    datafilemanager.cpp \
    rawdatahandlemanager.cpp \
    datahandler.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    devicetest.h \
    datafilemanager.h \
    rawdatahandlemanager.h \
    datahandler.h

