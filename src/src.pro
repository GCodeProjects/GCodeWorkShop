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
    qextserialbase.cpp \
    qextserialport.cpp \
    basic_interpreter.cpp
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
    qextserialbase.h \
    qextserialport.h \
    basic_interpreter.h
TEMPLATE = app
CONFIG += warn_on \
    thread \
    qt \
    debug
QT *= network
TARGET = ../bin/edytornc
RESOURCES = application.qrc
RC_FILE = edytornc.rc
CONFIG -= release
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
unix:HEADERS += posix_qextserialport.h
unix:SOURCES += posix_qextserialport.cpp
unix:DEFINES += _TTY_POSIX_
win32:HEADERS += win_qextserialport.h
win32:SOURCES += win_qextserialport.cpp
win32:DEFINES += _TTY_WIN_
unix:VERSION = 2009.00
