
VERSION = 2018.07
TEMPLATE = app
TARGET = edytornc

QT *= widgets printsupport serialport network
MODULES *= kdiff3 qtsingleapplication edytornc-common sfs addons

include(../common.pri)

INCLUDEPATH += include


# EdytorNC : public API
#######################################

HEADERS += \
    include/documentinfo.h \
    include/edytornc.h \
    include/mdichild.h \
    include/utils/expressionparser.h \
    include/utils/removezeros.h


# Private headers
#######################################

HEADERS += \
    findinf.h \
    gcoder.h \
    gcoderinfo.h \
    highlighter.h \
    newfiledialog.h \
    recentfiles.h \
    sessiondialog.h \
    sessionmanager.h \
    sessionnamedialog.h \
    setupdialog.h \
    tooltips.h

SOURCES += \
    documentinfo.cpp \
    edytornc.cpp \
    findinf.cpp \
    gcoderinfo.cpp \
    highlighter.cpp \
    main.cpp \
    mdichild.cpp \
    newfiledialog.cpp \
    recentfiles.cpp \
    sessiondialog.cpp \
    sessionmanager.cpp \
    sessionnamedialog.cpp \
    setupdialog.cpp \
    src/utils/expressionparser.cpp \
    src/utils/removezeros.cpp

FORMS += \
    edytornc.ui \
    findinfilesdialog.ui \
    mdichildform.ui \
    newfiledialog.ui \
    sessiondialog.ui \
    sessionnamedialog.ui \
    setupdialog.ui


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

RC_ICONS = images/edytornc.ico images/edytornc48.ico images/ncfile.ico
