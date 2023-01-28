
VERSION = 2018.07
TEMPLATE = app
TARGET = edytornc
CONFIG += qt

include(../common.pri)

QT *= widgets printsupport serialport network

INCLUDEPATH += include ../src-common ../src-common/include/ $$shadowed(../src-common) ../sfs
LIBS += -lkdiff3 -lqtsingleapplication -ledytornc-common

#EdytorNC

SOURCES += edytornc.cpp \
    main.cpp \
    highlighter.cpp \
    mdichild.cpp \
    dialogs.cpp \
    findinf.cpp \
    basic_interpreter.cpp \
    cleanupdialog.cpp \
    swapaxesdialog.cpp \
    newfiledialog.cpp \
    sessiondialog.cpp

HEADERS += include/edytornc.h \
    highlighter.h \
    mdichild.h \
    dialogs.h \
    findinf.h \
    basic_interpreter.h \
    cleanupdialog.h \
    tooltips.h \
    swapaxesdialog.h \
    newfiledialog.h \
    sessiondialog.h

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
    edytornc.ui \
    cleanupdialog.ui \
    swapaxesdialog.ui \
    newfiledialog.ui \
    sessiondialog.ui \
    newsessiondialog.ui

# resources
#######################################

RC_FILE = edytornc.rc


# translations
#######################################

TRANSLATIONS = $$tsFiles()


# target platforms dependencies
#######################################

unix {
    target.path = $${PREFIX}/bin
    INSTALLS += target
}

macx {
    LIBS += -framework IOKit -framework CoreFoundation
}

win32 {
    target.path = $${PREFIX}/bin
    INSTALLS += target
}
