
include(../common.pri)

TEMPLATE = app
DESTDIR = $${BIN_PATH}/
TARGET = edytornc

VERSION = 2018.07

CONFIG += qt

QT *= widgets printsupport serialport network

# moc doesn't detect Q_OS_LINUX correctly, so add this to make it work
linux*:DEFINES += __linux__

#EdytorNC

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
    mergeresultwindow.cpp \
    cleanupdialog.cpp \
    swapaxesdialog.cpp \
    newfiledialog.cpp \
    sessiondialog.cpp \
    serialtransmissiondialog.cpp \
    serialportconfigdialog.cpp \
    commapp.cpp \
    serialportcfghelpdialog.cpp \
    filechecker.cpp

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
    difftextwindow.h \
    diff.h \
    merger.h \
    optiondialog.h \
    common.h \
    fileaccess.h \
    kdiff3.h \
    gnudiff_system.h \
    gnudiff_diff.h \
    mergeresultwindow.h \
    cleanupdialog.h \
    tooltips.h \
    swapaxesdialog.h \
    newfiledialog.h \
    sessiondialog.h \
    serialtransmissiondialog.h \
    serialportconfigdialog.h \
    commapp.h \
    serialportcfghelpdialog.h \
    filechecker.h

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

TRANSLATIONS = edytornc_pl.ts \
    edytornc_ca.ts \
    edytornc_de.ts \
    edytornc_fi.ts \
    edytornc_cs_CZ.ts \
    edytornc_es.ts

RESOURCES = application.qrc
RC_FILE = edytornc.rc

QMAKE_EXTRA_TARGETS += update debug release translate

update.commands = lupdate src.pro
update.depends = $$SOURCES $$HEADERS $$FORMS $$TRANSLATIONS
release.commands = lrelease src.pro
release.depends = update
debug.commands = lrelease src.pro
debug.depends = update
translate.path = $${PREFIX}/share/edytornc/lang/
translate.files = ./*.qm
translate.depends = release
examples.files = ../examples/*
doc.files = ../README.md

unix {
    mime.files = ../install/linux/application-x-g-code.xml
    mimetypes.files = ../install/images/application-x-g-code.png
    desktop.files = ../install/linux/edytornc.desktop
    icon.files = ../install/linux/images/edytornc.png
    examples.path = $${PREFIX}/share/edytornc/examples
    doc.path = $${PREFIX}/share/doc/edytornc/
    UNAME = $$system(uname -a)
    contains( UNAME, x86_64 ):TARGET = ../bin/x86_64/edytornc
    target.path = $${PREFIX}/bin
    mime.path = $${PREFIX}/share/mime/packages/
    desktop.path = $${PREFIX}/share/applications/
    mimetypes.path = $${PREFIX}/share/icons/hicolor/32x32/mimetypes/
    icon.path = $${PREFIX}/share/icons/hicolor/48x48/apps/
    INSTALLS += target translate mime desktop mimetypes icon examples doc
}

macx {
    LIBS += -framework IOKit -framework CoreFoundation
}

win32 {
    DEFINES += WINVER=0x0501 # needed for mingw to pull in appropriate dbt business...probably a better way to do this
    LIBS += -lsetupapi -ladvapi32 -luser32
}
