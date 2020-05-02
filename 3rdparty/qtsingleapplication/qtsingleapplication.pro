# part of https://github.com/qtproject/qt-solutions/tree/master/qtsingleapplication

TEMPLATE = lib
TARGET = qtsingleapplication
CONFIG += qt staticlib

include(../../common.pri)

QT *= widgets network

SOURCES += \
    qtsingleapplication.cpp \
    qtlocalpeer.cpp \
    qtlockedfile_win.cpp \
    qtlockedfile_unix.cpp \
    qtlockedfile.cpp

HEADERS += \
    qtsingleapplication.h \
    qtlockedfile.h \
    qtlocalpeer.h
