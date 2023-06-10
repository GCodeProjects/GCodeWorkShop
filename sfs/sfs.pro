
VERSION = 0.1.0.0
TEMPLATE = app
TARGET = sfs

include(../common.pri)

QT *= widgets serialport network
# QTextCodec QRegExp uses in kdiff3. In Qt6 need add core5compat
greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat

INCLUDEPATH += ../src-common ../src-common/include/ $$shadowed(../src-common)
LIBS += -lkdiff3 -lqtsingleapplication -ledytornc-common

#EdytorNC - SFS

SOURCES = main.cpp commapp.cpp filechecker.cpp
HEADERS = commapp.h filechecker.h
FORMS = commapp.ui filechecker.ui


# target platforms dependencies
#######################################

unix {
    target.path = $${PREFIX}/bin
    INSTALLS += target
}

macx {
}

win32 {
    target.path = $${PREFIX}/bin
    INSTALLS += target
}

RC_ICONS = images/edytornc.ico images/edytornc48.ico
