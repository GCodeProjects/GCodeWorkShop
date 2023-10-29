
VERSION = 2023.03
TEMPLATE = lib
TARGET = addons
CONFIG += staticlib

QT *= widgets
MODULES *= edytornc edytornc-common basic

include(../common.pri)

INCLUDEPATH += include

HEADERS += \
    include/addons-actions.h

SOURCES += \
    src/addons-actions.cpp
