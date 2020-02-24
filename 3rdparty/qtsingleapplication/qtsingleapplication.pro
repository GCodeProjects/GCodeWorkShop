# part of https://github.com/qtproject/qt-solutions/tree/master/qtsingleapplication

include(../../common.pri)

TEMPLATE = lib
CONFIG += qt staticlib
TARGET = qtsingleapplication
DESTDIR = $${IMPORT_LIBS_PATH}/ # or BIN_PATH for shared lib

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
