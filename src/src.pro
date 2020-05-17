
VERSION = 2018.07
TEMPLATE = app
TARGET = edytornc
CONFIG += qt

include(../common.pri)

QT *= widgets printsupport serialport network

INCLUDEPATH += include/

#EdytorNC

SOURCES += edytornc.cpp \
    main.cpp \
    highlighter.cpp \
    mdichild.cpp \
    dialogs.cpp \
    findinf.cpp \
    serialtransmission.cpp \
    basic_interpreter.cpp \
    cleanupdialog.cpp \
    swapaxesdialog.cpp \
    newfiledialog.cpp \
    sessiondialog.cpp \
    serialtransmissiondialog.cpp \
    serialportconfigdialog.cpp \
    commapp.cpp \
    serialportcfghelpdialog.cpp \
    filechecker.cpp

HEADERS += include/edytornc.h \
    highlighter.h \
    mdichild.h \
    commoninc.h \
    dialogs.h \
    findinf.h \
    serialtransmission.h \
    basic_interpreter.h \
    cleanupdialog.h \
    tooltips.h \
    swapaxesdialog.h \
    newfiledialog.h \
    sessiondialog.h \
    serialtransmissiondialog.h \
    serialportconfigdialog.h \
    commapp.h \
    serialportcfghelpdialog.h \
    filechecker.h \
    include/generalconfig.h

FORMS += i2mdialog.ui \
    feedsdialog.ui \
    renumberdialog.ui \
    dotdialog.ui \
    triangledialog.ui \
    bhctabform.ui \
    bhcdialog.ui \
    mdichildform.ui \
    findinfilesdialog.ui \
    chamferdialog.ui \
    i2mprogdialog.ui \
    setupdialog.ui \
    transmissiondialog.ui \
    edytornc.ui \
    cleanupdialog.ui \
    swapaxesdialog.ui \
    newfiledialog.ui \
    sessiondialog.ui \
    newsessiondialog.ui \
    serialtransmissiondialog.ui \
    serialportconfigdialog.ui \
    commapp.ui \
    serialportcfghelpdialog.ui \
    filechecker.ui


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
RC_FILE = edytornc.rc


# translations
#######################################

TRANSLATIONS = $$tsFiles()


# target platforms dependencies
#######################################

unix {
    !defined(PREFIX, var) {
        PREFIX = /usr/local/
        message(seting PREFIX = $$PREFIX)
    }

    target.path = $${PREFIX}/bin

    INSTALLS += target
}

macx {
    LIBS += -framework IOKit -framework CoreFoundation
}

win32 {
}


# static libs from subprojects
#######################################

LIBS += -lkdiff3 -lqtsingleapplication
