
TEMPLATE = lib
TARGET = basic
CONFIG += staticlib

USE -= check_iwyu

include(../../common.pri)

HEADERS += basic_interpreter.h
SOURCES += basic_interpreter.cpp
