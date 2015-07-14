
##qextserialport

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

#PUBLIC_HEADERS         += $$PWD/qextserialport.h \
#                          $$PWD/qextserialenumerator.h \
#                          $$PWD/qextserialport_global.h

#HEADERS                += $$PUBLIC_HEADERS \
#                          $$PWD/qextserialport_p.h \
#                          $$PWD/qextserialenumerator_p.h \


#SOURCES                += $$PWD/qextserialport.cpp \
#                          $$PWD/qextserialenumerator.cpp \

#unix {
#    SOURCES            += $$PWD/qextserialport_unix.cpp
#    linux* {
#        SOURCES        += $$PWD/qextserialenumerator_linux.cpp
#    } else:macx {
#        SOURCES        += $$PWD/qextserialenumerator_osx.cpp
#    } else {
#        SOURCES        += $$PWD/qextserialenumerator_unix.cpp
#    }
#}
#win32:SOURCES          += $$PWD/qextserialport_win.cpp \
#                          $$PWD/qextserialenumerator_win.cpp

#linux*{
#    !qesp_linux_udev:DEFINES += QESP_NO_UDEV
#    qesp_linux_udev: LIBS += -ludev
#}

macx:LIBS              += -framework IOKit -framework CoreFoundation
win32:LIBS             += -lsetupapi -ladvapi32 -luser32

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
    newfiledialog.cpp
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
    newfiledialog.h
TEMPLATE = app
CONFIG += warn_on \
    thread \
    qt
#CONFIG += release
QT *= network
QT += widgets
QT += printsupport
QT += serialport
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
    edytornc.ui \
    cleanupdialog.ui \
    swapaxesdialog.ui \
    newfiledialog.ui
#TRANSLATIONS = edytornc_pl.ts \
#    edytornc_ca.ts \
#    edytornc_de.ts \
#    edytornc_fi.ts \
#    edytornc_cs_CZ.ts \
#    edytornc_es.ts
OBJECTS_DIR = build/obj
MOC_DIR = build/moc
unix:update.commands = lupdate src.pro
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
unix:contains( UNAME, x86_64 ):TARGET = ../bin/x86_64/edytornc
unix:target.path = $${PREFIX}/bin
unix:mime.path = $${PREFIX}/share/mime/packages/
unix:mime.files = application-x-g-code.xml
unix:desktop.path = $${PREFIX}/share/applications/
unix:desktop.files = edytornc.desktop
unix:mimetypes.path = $${PREFIX}/share/icons/hicolor/32x32/mimetypes/
unix:mimetypes.files = images/application-x-g-code.png
unix:icon.path = $${PREFIX}/share/icons/hicolor/48x48/apps/
unix:icon.files = images/edytornc.png    
unix:INSTALLS += target translate mime desktop mimetypes icon examples doc
macx:LIBS += -framework IOKit -framework CoreFoundation
win32:DEFINES += WINVER=0x0501 # needed for mingw to pull in appropriate dbt business...probably a better way to do this
win32:LIBS += -lsetupapi
win32:TARGET = ../bin/edytornc
VERSION = 2015.07
