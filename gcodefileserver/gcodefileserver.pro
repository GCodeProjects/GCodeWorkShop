# GCoderFileServer

TEMPLATE = app
TARGET = gcodefileserver

QT *= widgets serialport network
MODULES *= kdiff3 qtsingleapplication GCodeShared

include(../common.pri)

defined(VERSION, var) {
    # Define the version as a string literal
    # https://stackoverflow.com/a/2411008
    DEFINES += 'GCODEWORKSHOP_VERSION=\\"$$VERSION\\"'
} else {
    VERSION = $$getVersion()
}

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
    QMAKE_TARGET_PRODUCT=GCodeFileServer
}

RC_ICONS = images/edytornc.ico images/edytornc48.ico
