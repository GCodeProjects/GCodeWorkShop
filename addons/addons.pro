
VERSION = 2023.03
TEMPLATE = lib
TARGET = addons
CONFIG += staticlib

QT *= widgets
MODULES *= edytornc edytornc-common basic

include(../common.pri)

INCLUDEPATH += include

HEADERS += \
    include/addons-actions.h \
    src/addons-context.h

SOURCES += \
    src/addons-actions.cpp \
    src/addons-context.cpp

include(src/bhc/bhc.pri)
include(src/blockskip/blockskip.pri)
include(src/chamfer/chamfer.pri)
include(src/cleanup/cleanup.pri)
include(src/comment/comment.pri)
