SOURCES += edytornc.cpp \
    main.cpp \
    highlighter.cpp \
    mdichild.cpp \
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
    qextserialenumerator.cpp \
    difftextwindow.cpp \
    diff.cpp \
    merger.cpp \
    optiondialog.cpp \
    common.cpp \
    fileaccess.cpp \
    kdiff3.cpp \
    gnudiff_xmalloc.cpp \
    gnudiff_io.cpp \
    gnudiff_analyze.cpp \
    pdiff.cpp \
    mergeresultwindow.cpp
HEADERS += edytornc.h \
    highlighter.h \
    mdichild.h \
    commoninc.h \
    dialogs.h \
    findinf.h \
    qtsingleapplication.h \
    qtlockedfile.h \
    qtlocalpeer.h \
    serialtransmission.h \
    basic_interpreter.h \
    qextserialport.h \
    qextserialenumerator.h \
    difftextwindow.h \
    diff.h \
    merger.h \
    optiondialog.h \
    common.h \
    fileaccess.h \
    kdiff3.h \
    gnudiff_system.h \
    gnudiff_diff.h \
    mergeresultwindow.h
TEMPLATE = app
CONFIG += warn_on \
    thread \
    qt
CONFIG += release
QT *= network
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
    transmissiondialog.ui \
    transprogressdialog.ui \
    edytornc.ui
TRANSLATIONS = edytornc_pl.ts \
    edytornc_ca.ts \
    edytornc_de.ts \
    edytornc_fi.ts \
    edytornc_cs_CZ.ts
OBJECTS_DIR = build/obj
MOC_DIR = build/moc
unix:SOURCES += posix_qextserialport.cpp
unix:TARGET = ../bin/edytornc
unix:UNAME = $$system(uname -a)
unix:contains( UNAME, x86_64 ):TARGET = ../bin/x86_64/edytornc
macx:LIBS += -framework IOKit -framework CoreFoundation
win32:SOURCES += win_qextserialport.cpp
win32:DEFINES += WINVER=0x0501 # needed for mingw to pull in appropriate dbt business...probably a better way to do this
win32:LIBS += -lsetupapi
win32:TARGET = ../bin/edytornc
VERSION = 2011.01
