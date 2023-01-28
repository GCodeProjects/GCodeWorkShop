
VERSION = 0.1.0.0
TEMPLATE = app
TARGET = sfs
CONFIG += qt

include(../common.pri)

QT *= widgets serialport network

INCLUDEPATH += ../src-common ../src-common/include/ $$shadowed(../src-common)
LIBS += -lkdiff3 -lqtsingleapplication -ledytornc-common

#EdytorNC - SFS

SOURCES = main.cpp commapp.cpp filechecker.cpp
HEADERS = commapp.h filechecker.h
FORMS = commapp.ui filechecker.ui
RC_FILE = sfs.rc

#TRANSLATIONS = $$tsFiles()


# target platforms dependencies
#######################################

unix {
    target.path = $${PREFIX}/bin
    INSTALLS += target
}

macx {
    LIBS += -framework IOKit -framework CoreFoundation
}

win32 {
    target.path = $${PREFIX}/bin
    INSTALLS += target
}
