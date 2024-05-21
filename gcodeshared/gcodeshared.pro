
VERSION = 2018.07
TEMPLATE = lib
TARGET = gcodeshared
CONFIG += staticlib

QT *= widgets serialport network

include(../common.pri)

INCLUDEPATH += include

# GCoderShared

SOURCES += \
    serialporttestdialog.cpp \
    serialtransmissiondialog.cpp \
    serialportconfigdialog.cpp

HEADERS += \
    include/version.h \
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
    include/utils/guessfilename.h \
    include/utils/splitfile.h \
    utils/filepatterns.h

SOURCES += ui/longjobhelper.cpp \
    utils/medium.cpp \
    utils/configpage.cpp \
    utils/configdialog.cpp \
    utils/guessfilename.cpp \
    utils/splitfile.cpp

FORMS += utils/configdialog.ui


# resources
#######################################

RESOURCES = application.qrc
