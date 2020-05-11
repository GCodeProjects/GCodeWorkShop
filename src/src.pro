
VERSION = 2018.07
TEMPLATE = app
TARGET = edytornc
CONFIG += qt

include(../common.pri)

QT *= widgets printsupport serialport network

INCLUDEPATH += include/

#EdytorNC

SOURCES += edytornc.cpp \
    main.cpp \
    highlighter.cpp \
    mdichild.cpp \
    dialogs.cpp \
    findinf.cpp \
    serialtransmission.cpp \
    basic_interpreter.cpp \
    cleanupdialog.cpp \
    swapaxesdialog.cpp \
    newfiledialog.cpp \
    sessiondialog.cpp \
    serialtransmissiondialog.cpp \
    serialportconfigdialog.cpp \
    commapp.cpp \
    serialportcfghelpdialog.cpp \
    filechecker.cpp

HEADERS += include/edytornc.h \
    highlighter.h \
    mdichild.h \
    commoninc.h \
    dialogs.h \
    findinf.h \
    serialtransmission.h \
    basic_interpreter.h \
    cleanupdialog.h \
    tooltips.h \
    swapaxesdialog.h \
    newfiledialog.h \
    sessiondialog.h \
    serialtransmissiondialog.h \
    serialportconfigdialog.h \
    commapp.h \
    serialportcfghelpdialog.h \
    filechecker.h \
    include/generalconfig.h

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

HEADERS += include/ui/longjobhelper.h \
    include/utils/config.h \
    include/utils/medium.h \
    include/utils/configpage.h \
    include/utils/configdialog.h

SOURCES += ui/longjobhelper.cpp \
    utils/config.cpp \
    utils/medium.cpp \
    utils/configpage.cpp \
    utils/configdialog.cpp

unix:SOURCES += utils/medium_linux.cpp
win32:SOURCES += utils/medium_win.cpp

FORMS += utils/configdialog.ui

# resources
#######################################

RESOURCES = application.qrc
RC_FILE = edytornc.rc


# translations
#######################################

TRANSLATIONS = \
    $${LANG_PATH}/edytornc_pl.ts \
    $${LANG_PATH}/edytornc_ca.ts \
    $${LANG_PATH}/edytornc_de.ts \
    $${LANG_PATH}/edytornc_fi.ts \
    $${LANG_PATH}/edytornc_cs_CZ.ts \
    $${LANG_PATH}/edytornc_es.ts \
    $${LANG_PATH}/edytornc_nl.ts \
    $${LANG_PATH}/edytornc_ru.ts

QMAKE_EXTRA_TARGETS += langupdate langrelease translate

langupdate.commands = lupdate src.pro
langrelease.target = lang_update
langrelease.commands = lrelease src.pro
langrelease.depends = langupdate
PRE_TARGETDEPS += lang_update

translate.files = ../lang/*.qm
#translate.depends = langrelease


# other files
#######################################

examples.files = ../examples/*
doc.files = ../README.md


# target platforms dependencies
#######################################

unix {
    !defined(PREFIX, var) {
        PREFIX = /usr/local/
        message(seting PREFIX = $$PREFIX)
    }

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
}


# static libs from subprojects

LIBS += -lkdiff3 -lqtsingleapplication
