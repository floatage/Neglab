TEMPLATE = app

QT += qml quick serialport
CONFIG += c++11

SOURCES += main.cpp \
    devicetest.cpp \
    rawdatahandlemanager.cpp \
    datahandler.cpp \
    commonvariable.cpp \
    intermediateresulthook.cpp \
    datasourcemanager.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    devicetest.h \
    rawdatahandlemanager.h \
    datahandler.h \
    commonvariable.h \
    intermediateresulthook.h \
    datasourcemanager.h

