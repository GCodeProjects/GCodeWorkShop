
include(../common.pri)

TEMPLATE = app
DESTDIR = $${BIN_PATH}/
TARGET = edytornc

VERSION = 2018.07

CONFIG += qt

QT *= widgets printsupport serialport network

# moc doesn't detect Q_OS_LINUX correctly, so add this to make it work
linux*:DEFINES += __linux__

INCLUDEPATH += include/

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


# utils
#######################################

HEADERS += include/ui/longjobhelper.h

SOURCES += ui/longjobhelper.cpp


# resources
#######################################

RESOURCES = application.qrc
RC_FILE = edytornc.rc


# translations
#######################################

TRANSLATIONS = ../lang/edytornc_pl.ts \
    ../lang/edytornc_ca.ts \
    ../lang/edytornc_de.ts \
    ../lang/edytornc_fi.ts \
    ../lang/edytornc_cs_CZ.ts \
    ../lang/edytornc_es.ts

QMAKE_EXTRA_TARGETS += langupdate langrelease translate

langupdate.target = lang_update
langupdate.commands = lupdate src.pro
langupdate.depends = $$SOURCES $$HEADERS $$FORMS $$TRANSLATIONS
langrelease.commands = lrelease src.pro
#langrelease.depends = langupdate
translate.files = ../lang/*.qm
translate.depends = langrelease
#PRE_TARGETDEPS += lang_update


# other files
#######################################

examples.files = ../examples/*
doc.files = ../README.md


# target platforms dependencies
#######################################

unix {
    PREFIX = /usr/local/

    mime.files = ../install/linux/application-x-g-code.xml
    desktop.files = ../install/linux/edytornc.desktop
    examples.path = $${PREFIX}/share/edytornc/examples
    doc.path = $${PREFIX}/share/doc/edytornc/
    target.path = $${PREFIX}/bin
    mime.path = $${PREFIX}/share/mime/packages/
    desktop.path = $${PREFIX}/share/applications/
    translate.path = $${PREFIX}/share/edytornc/lang/

    ICONS_BASE_PATH = $$PROJECT_ROOT_PATH/install/linux/images/
    icons.base = $$ICONS_BASE_PATH
    icons.files = $$findFiles($$ICONS_BASE_PATH, *.png)
    icons.path = $${PREFIX}/share/icons/

    INSTALLS += target translate mime desktop icons examples doc
}

macx {
    LIBS += -framework IOKit -framework CoreFoundation
}

win32 {
    DEFINES += WINVER=0x0501 # needed for mingw to pull in appropriate dbt business...probably a better way to do this
    LIBS += -lsetupapi -ladvapi32 -luser32
}

