
VERSION = 2018.07
TEMPLATE = app
TARGET = edytornc

QT *= widgets printsupport serialport network
MODULES *= kdiff3 qtsingleapplication edytornc-common sfs addons basic

include(../common.pri)

INCLUDEPATH += include

#EdytorNC

SOURCES += edytornc.cpp \
    main.cpp \
    highlighter.cpp \
    mdichild.cpp \
    setupdialog.cpp \
    findinf.cpp \
    newfiledialog.cpp \
    sessiondialog.cpp \
    src/utils/expressionparser.cpp \
    src/utils/removezeros.cpp

HEADERS += include/edytornc.h \
    highlighter.h \
    include/mdichild.h \
    setupdialog.h \
    findinf.h \
    tooltips.h \
    newfiledialog.h \
    sessiondialog.h \
    include/utils/expressionparser.h \
    include/utils/removezeros.h

FORMS += mdichildform.ui \
    findinfilesdialog.ui \
    setupdialog.ui \
    edytornc.ui \
    newfiledialog.ui \
    sessiondialog.ui \
    newsessiondialog.ui


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
