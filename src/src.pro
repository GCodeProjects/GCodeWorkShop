
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
    include/document.h \
    include/documentinfo.h \
    include/documentmanager.h \
    include/documentstyle.h \
    include/documentproducer.h \
    include/documentwidgetproperties.h \
    include/edytornc.h \
    include/gcoderdocument.h \
    include/gcoderstyle.h \
    include/gcoderwidgetproperties.h \
    include/utils/expressionparser.h \
    include/utils/removezeros.h


# Private headers
#######################################

HEADERS += \
    capslockeventfilter.h \
    documentwidgetcloseeventfilter.h \
    findinf.h \
    gcoder.h \
    gcodereventfilter.h \
    gcoderinfo.h \
    gcoderproducer.h \
    highlighter.h \
    highlightmode.h \
    inlinecalc.h \
    newfiledialog.h \
    recentfiles.h \
    sessiondialog.h \
    sessionmanager.h \
    sessionnamedialog.h \
    setupdialog.h \
    tooltips.h

SOURCES += \
    capslockeventfilter.cpp \
    document.cpp \
    documentinfo.cpp \
    documentmanager.cpp \
    documentproducer.cpp \
    documentstyle.cpp \
    documentwidgetcloseeventfilter.cpp \
    documentwidgetproperties.cpp \
    edytornc.cpp \
    findinf.cpp \
    gcoderdocument.cpp \
    gcodereventfilter.cpp \
    gcoderinfo.cpp \
    gcoderproducer.cpp \
    gcoderstyle.cpp \
    gcoderwidgetproperties.cpp \
    highlighter.cpp \
    inlinecalc.cpp \
    main.cpp \
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
