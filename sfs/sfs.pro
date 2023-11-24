
VERSION = 0.1.0.0
TEMPLATE = app
TARGET = sfs

include(../common.pri)

QT *= widgets serialport network
MODULES *= kdiff3 qtsingleapplication edytornc-common

include(../common.pri)

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
