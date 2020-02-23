# part of http://kdiff3.sourceforge.net/

include(../../common.pri)

TEMPLATE = lib
CONFIG += qt staticlib
TARGET = kdiff3
DESTDIR = $${IMPORT_LIBS_PATH}/ # or BIN_PATH for shared lib

QT *= widgets

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
    mergeresultwindow.h

TRANSLATIONS = \
    $${LANG_PATH}/kdiff3_pl.ts \
    $${LANG_PATH}/kdiff3_ca.ts \
    $${LANG_PATH}/kdiff3_de.ts \
    $${LANG_PATH}/kdiff3_fi.ts \
    $${LANG_PATH}/kdiff3_cs.ts \
    $${LANG_PATH}/kdiff3_es.ts

QMAKE_EXTRA_TARGETS += langupdate
langupdate.target = lang_update
langupdate.commands = lupdate kdiff3.pro
langupdate.depends = $$SOURCES $$HEADERS $$TRANSLATIONS
PRE_TARGETDEPS += lang_update
