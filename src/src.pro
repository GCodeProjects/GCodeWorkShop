
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
    include/edytornc.h \
    include/mdichild.h \
    include/utils/expressionparser.h \
    include/utils/removezeros.h


# Private headers
#######################################

HEADERS += \
    findinf.h \
    highlighter.h \
    newfiledialog.h \
    sessiondialog.h \
    setupdialog.h \
    tooltips.h

SOURCES += \
    edytornc.cpp \
    findinf.cpp \
    highlighter.cpp \
    main.cpp \
    mdichild.cpp \
    newfiledialog.cpp \
    sessiondialog.cpp \
    setupdialog.cpp \
    src/utils/expressionparser.cpp \
    src/utils/removezeros.cpp

FORMS += \
    edytornc.ui \
    findinfilesdialog.ui \
    mdichildform.ui \
    newfiledialog.ui \
    newsessiondialog.ui \
    sessiondialog.ui \
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
