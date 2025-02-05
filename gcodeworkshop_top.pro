
TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS = 3rdparty gcodeshared addons gcodeworkshop gcodefileserver


include(common.pri)
include(lang/lang.pri)

examples.files = $$PROJECT_ROOT_PATH/examples/*
doc.files = $$PROJECT_ROOT_PATH/README.md $$PROJECT_ROOT_PATH/doc/SerialTransmission_Help.html


# target platforms dependencies
#######################################

unix {
    mime.files = $$PROJECT_ROOT_PATH/install/linux/application-x-g-code.xml
    desktop.files = $$PROJECT_ROOT_PATH/install/linux/gcodeworkshop.desktop
#   desktop.files += $$PROJECT_ROOT_PATH/install/linux/gcodefileserver.desktop

    examples.path = $${PREFIX}/share/gcodeworkshop/examples
    doc.path = $${PREFIX}/share/doc/gcodeworkshop
    mime.path = $${PREFIX}/share/mime/packages
    desktop.path = $${PREFIX}/share/applications

    ICONS_BASE_PATH = $$PROJECT_ROOT_PATH/install/linux/images
    icons.base = $$ICONS_BASE_PATH
    icons.files = $$findFiles($$ICONS_BASE_PATH, *.png)
    icons.path = $${PREFIX}/share/icons

    INSTALLS += mime desktop icons examples doc
}

macx {
}

win32 {
    examples.path = $${PREFIX}/examples
    doc.path = $${PREFIX}/doc

    INSTALLS += examples doc
}

!defined(VERSION, var): VERSION = $$getVersion()
message(Project version: $$VERSION)
