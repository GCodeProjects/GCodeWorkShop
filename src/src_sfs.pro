
INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD


macx:LIBS              += -framework IOKit -framework CoreFoundation
win32:LIBS             += -lsetupapi -ladvapi32 -luser32

# moc doesn't detect Q_OS_LINUX correctly, so add this to make it work
linux*:DEFINES += __linux__



#EdytorNC

SOURCES += main_sfs.cpp \
    qtsingleapplication.cpp \
    qtlocalpeer.cpp \
    qtlockedfile_win.cpp \
    qtlockedfile_unix.cpp \
    qtlockedfile.cpp \
    serialtransmission.cpp \
    difftextwindow.cpp \
    diff.cpp \
    merger.cpp \
    common.cpp \
    fileaccess.cpp \
    kdiff3.cpp \
    gnudiff_xmalloc.cpp \
    gnudiff_io.cpp \
    gnudiff_analyze.cpp \
    pdiff.cpp \
    mergeresultwindow.cpp \
    optiondialog.cpp \
    serialtransmissiondialog.cpp \
    serialportconfigdialog.cpp \
    commapp.cpp \
    serialportcfghelpdialog.cpp \
    filechecker.cpp
HEADERS += commoninc.h \
    qtsingleapplication.h \
    qtlockedfile.h \
    qtlocalpeer.h \
    serialtransmission.h \
    difftextwindow.h \
    diff.h \
    merger.h \
    common.h \
    fileaccess.h \
    kdiff3.h \
    gnudiff_system.h \
    gnudiff_diff.h \
    mergeresultwindow.h \
    optiondialog.h \
    serialtransmissiondialog.h \
    serialportconfigdialog.h \
    commapp.h \
    serialportcfghelpdialog.h \
    filechecker.h
TEMPLATE = app
CONFIG += warn_on \
    thread \
    qt
#CONFIG += CONSOLE
#CONFIG += release
QT *= network
QT += widgets
QT += printsupport
QT += serialport
RESOURCES = application.qrc
RC_FILE = edytornc.rc
FORMS += transmissiondialog.ui \
    edytornc.ui \
    serialtransmissiondialog.ui \
    serialportconfigdialog.ui \
    commapp.ui \
    serialportcfghelpdialog.ui \
    filechecker.ui
#TRANSLATIONS = edytornc_pl.ts \
#    edytornc_ca.ts \
#    edytornc_de.ts \
#    edytornc_fi.ts \
#    edytornc_cs_CZ.ts \
#    edytornc_es.ts
OBJECTS_DIR = build/obj
MOC_DIR = build/moc
unix:update.commands = lupdate src_sfs.pro
unix:update.depends = $$SOURCES $$HEADERS $$FORMS $$TRANSLATIONS
unix:release.commands = lrelease src.pro
unix:release.depends = update
unix:translate.path = $${PREFIX}/share/edytornc/lang/
unix:translate.files = ./*.qm
unix:translate.depends = release
unix:examples.files = ../EXAMPLES/*
unix:examples.path = $${PREFIX}/share/edytornc/EXAMPLES
unix:doc.files = ../ReadMe
unix:doc.path = $${PREFIX}/share/doc/edytornc/
unix:QMAKE_EXTRA_TARGETS += update release translate
unix:TARGET = edytornc
unix:UNAME = $$system(uname -a)
unix:contains( UNAME, x86_64 ):TARGET = ../bin/x86_64/sfs
unix:target.path = $${PREFIX}/bin
unix:mime.path = $${PREFIX}/share/mime/packages/
unix:mime.files = application-x-g-code.xml
unix:desktop.path = $${PREFIX}/share/applications/
unix:desktop.files = edytornc.desktop
unix:mimetypes.path = $${PREFIX}/share/icons/hicolor/32x32/mimetypes/
unix:mimetypes.files = images/application-x-g-code.png
unix:icon.path = $${PREFIX}/share/icons/hicolor/48x48/apps/
unix:icon.files = images/serial.png
unix:INSTALLS += target translate mime desktop mimetypes icon examples doc
macx:LIBS += -framework IOKit -framework CoreFoundation
win32:DEFINES += WINVER=0x0501 # needed for mingw to pull in appropriate dbt business...probably a better way to do this
win32:LIBS += -lsetupapi
win32:TARGET = ../bin/sfs
VERSION = 2015.11
