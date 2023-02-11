
VERSION = 2018.07
TEMPLATE = lib
TARGET = edytornc-common
CONFIG += qt staticlib

include(../common.pri)

QT *= widgets serialport network

INCLUDEPATH += include

#EdytorNC common

SOURCES += \
    serialtransmission.cpp \
    serialtransmissiondialog.cpp \
    serialportconfigdialog.cpp \
    serialportcfghelpdialog.cpp

HEADERS += \
    commoninc.h \
    serialportsettings.h \
    serialtransmission.h \
    serialtransmissiondialog.h \
    serialportconfigdialog.h \
    serialportcfghelpdialog.h \
    serialportsettings.h \
    include/generalconfig.h

FORMS += \
    transmissiondialog.ui \
    serialtransmissiondialog.ui \
    serialportconfigdialog.ui \
    serialportcfghelpdialog.ui


# utils
#######################################

HEADERS += include/ui/longjobhelper.h \
    include/utils/config.h \
    include/utils/medium.h \
    include/utils/configpage.h \
    include/utils/configdialog.h

SOURCES += ui/longjobhelper.cpp \
    utils/config.cpp \
    utils/medium.cpp \
    utils/configpage.cpp \
    utils/configdialog.cpp

unix:SOURCES += utils/medium_linux.cpp
win32:SOURCES += utils/medium_win.cpp

FORMS += utils/configdialog.ui


# resources
#######################################

RESOURCES = application.qrc
