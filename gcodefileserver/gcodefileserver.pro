# GCoderFileServer

VERSION = 0.1.0.0
TEMPLATE = app
TARGET = gcodefileserver

include(../common.pri)

QT *= widgets serialport network
MODULES *= kdiff3 qtsingleapplication GCodeShared

include(../common.pri)


SOURCES = \
    filechecker.cpp \
    gcodefileserver.cpp \
    main.cpp

HEADERS = \
    filechecker.h \
    gcodefileserver.h

FORMS = \
    filechecker.ui \
    gcodefileserver.ui


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
