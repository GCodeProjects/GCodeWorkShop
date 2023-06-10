# part of http://kdiff3.sourceforge.net/

TEMPLATE = lib
TARGET = kdiff3
CONFIG += staticlib

include(../../common.pri)

QT *= widgets
# QTextCodec QRegExp in Qt6
greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat

SOURCES += \
    difftextwindow.cpp \
    diff.cpp \
    merger.cpp \
    optiondialog.cpp \
    common.cpp \
    fileaccess.cpp \
    kdiff3.cpp \
    gnudiff_xmalloc.cpp \
    gnudiff_io.cpp \
    gnudiff_analyze.cpp \
    pdiff.cpp \
    mergeresultwindow.cpp

HEADERS += \
    difftextwindow.h \
    diff.h \
    merger.h \
    optiondialog.h \
    common.h \
    fileaccess.h \
    kdiff3.h \
    gnudiff_system.h \
    gnudiff_diff.h \
    mergeresultwindow.h \
    compatibility.h
