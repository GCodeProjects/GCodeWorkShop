SOURCES += edytornc.cpp \
    main.cpp \
    highlighter.cpp \
    mdichild.cpp \
    customfiledialog.cpp \
    dialogs.cpp \
    findinf.cpp \
    qtsingleapplication.cpp \
    qtlocalpeer.cpp \
    qtlockedfile_win.cpp \
    qtlockedfile_unix.cpp \
    qtlockedfile.cpp \
    serialtransmission.cpp \
    basic_interpreter.cpp \
    qextserialport.cpp \
    qextserialenumerator.cpp

HEADERS += edytornc.h \
    highlighter.h \
    mdichild.h \
    commoninc.h \
    customfiledialog.h \
    dialogs.h \
    findinf.h \
    qtsingleapplication.h \
    qtlockedfile.h \
    qtlocalpeer.h \
    serialtransmission.h \
    basic_interpreter.h \
    qextserialport.h \
    qextserialenumerator.h

TEMPLATE = app
CONFIG += warn_on \
    thread \
    qt \
    debug
CONFIG -= release
QT *= network
TARGET = ../bin/edytornc
RESOURCES = application.qrc
RC_FILE = edytornc.rc
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
    spconfigdialog.ui \
    transmissiondialog.ui
TRANSLATIONS = edytornc_pl.ts
OBJECTS_DIR = build/obj
MOC_DIR = build/moc

unix:SOURCES           += posix_qextserialport.cpp
macx: LIBS             += -framework IOKit

win32:SOURCES          += win_qextserialport.cpp
win32:DEFINES          += WINVER=0x0501 # needed for mingw to pull in appropriate dbt business...probably a better way to do this
win32:LIBS             += -lsetupapi


unix:VERSION = 2009.12




