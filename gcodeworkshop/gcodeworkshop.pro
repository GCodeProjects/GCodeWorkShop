
TEMPLATE = app
TARGET = gcodeworkshop

QT *= widgets printsupport serialport network
MODULES *= kdiff3 qtsingleapplication GCodeShared GCodeFileServer addons

include(../common.pri)

defined(VERSION, var) {
    # Define the version as a string literal
    # https://stackoverflow.com/a/2411008
    DEFINES += 'GCODEWORKSHOP_VERSION=\\"$$VERSION\\"'
} else {
    VERSION = $$getVersion()
}

INCLUDEPATH += include


# GCoderWorkShop : public API
#######################################

HEADERS += \
    include/document.h \
    include/documentinfo.h \
    include/documentmanager.h \
    include/documentstyle.h \
    include/documentproducer.h \
    include/documentwidgetproperties.h \
    include/gcoderdocument.h \
    include/gcoderstyle.h \
    include/gcoderwidgetproperties.h \
    include/gcodeworkshop.h \
    include/utils/expressionparser.h \
    include/utils/removezeros.h


# Private headers
#######################################

HEADERS += \
    src/capslockeventfilter.h \
    src/documentwidgetcloseeventfilter.h \
    src/findinf.h \
    src/gcoder.h \
    src/gcodereventfilter.h \
    src/gcoderinfo.h \
    src/gcoderproducer.h \
    src/highlighter.h \
    src/highlightmode.h \
    src/inlinecalc.h \
    src/newfiledialog.h \
    src/recentfiles.h \
    src/sessiondialog.h \
    src/sessionmanager.h \
    src/sessionnamedialog.h \
    src/setupdialog.h \
    src/tooltips.h

SOURCES += \
    src/capslockeventfilter.cpp \
    src/document.cpp \
    src/documentinfo.cpp \
    src/documentmanager.cpp \
    src/documentproducer.cpp \
    src/documentstyle.cpp \
    src/documentwidgetcloseeventfilter.cpp \
    src/documentwidgetproperties.cpp \
    src/findinf.cpp \
    src/gcoderdocument.cpp \
    src/gcodereventfilter.cpp \
    src/gcoderinfo.cpp \
    src/gcoderproducer.cpp \
    src/gcoderstyle.cpp \
    src/gcoderwidgetproperties.cpp \
    src/gcodeworkshop.cpp \
    src/highlighter.cpp \
    src/inlinecalc.cpp \
    src/main.cpp \
    src/newfiledialog.cpp \
    src/recentfiles.cpp \
    src/sessiondialog.cpp \
    src/sessionmanager.cpp \
    src/sessionnamedialog.cpp \
    src/setupdialog.cpp \
    src/utils/expressionparser.cpp \
    src/utils/removezeros.cpp

FORMS += \
    src/findinfilesdialog.ui \
    src/gcodeworkshop.ui \
    src/newfiledialog.ui \
    src/sessiondialog.ui \
    src/sessionnamedialog.ui \
    src/setupdialog.ui


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
    QMAKE_TARGET_PRODUCT=GCodeWorkShop
}

RC_ICONS = images/edytornc.ico images/edytornc48.ico images/ncfile.ico
