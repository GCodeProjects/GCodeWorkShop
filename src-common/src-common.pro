
VERSION = 2018.07
TEMPLATE = lib
TARGET = edytornc-common
CONFIG += staticlib

QT *= widgets serialport network

include(../common.pri)

INCLUDEPATH += include

#EdytorNC common

SOURCES += \
    serialporttestdialog.cpp \
    serialtransmissiondialog.cpp \
    serialportconfigdialog.cpp

HEADERS += \
    commoninc.h \
    serialportsettings.h \
    serialporttestdialog.h \
    serialtransmissiondialog.h \
    serialportconfigdialog.h \
    serialportsettings.h

FORMS += \
    serialporttestdialog.ui \
    serialtransmissiondialog.ui \
    serialportconfigdialog.ui


# utils
#######################################

HEADERS += include/ui/longjobhelper.h \
    include/utils/medium.h \
    include/utils/configpage.h \
    include/utils/configdialog.h \
    include/utils/splitfile.h

SOURCES += ui/longjobhelper.cpp \
    utils/medium.cpp \
    utils/configpage.cpp \
    utils/configdialog.cpp \
    utils/splitfile.cpp

FORMS += utils/configdialog.ui


# resources
#######################################

RESOURCES = application.qrc
