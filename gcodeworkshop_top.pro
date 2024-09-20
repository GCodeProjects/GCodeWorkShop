
TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS = 3rdparty gcodeshared addons gcodeworkshop gcodefileserver


include(common.pri)
include(install/install.pri)

QMAKE_EXTRA_TARGETS += lupdate
lupdate.depends = lupdateGCodeWorkShop lupdateKdiff3
QMAKE_EXTRA_TARGETS += lupdateGCodeWorkShop lupdateKdiff3
lupdateGCodeWorkShop.commands = $$[QT_INSTALL_BINS]/lupdate \
    $$PROJECT_ROOT_PATH/gcodeworkshop \
    $$PROJECT_ROOT_PATH/gcodeshared \
    $$PROJECT_ROOT_PATH/gcodefileserver \
    $$PROJECT_ROOT_PATH/addons \
    -ts $$tsFiles(gcodeworkshop)
lupdateKdiff3.commands = $$[QT_INSTALL_BINS]/lupdate \
    $$PROJECT_ROOT_PATH/3rdparty/kdiff3 \
    -ts $$tsFiles(kdiff3)

QMAKE_EXTRA_TARGETS += lrelease
lrelease.commands = $$[QT_INSTALL_BINS]/lrelease \
    $$tsFiles(gcodeworkshop) \
    $$tsFiles(kdiff3)

translate.files = $$LANG_PATH/*.qm
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
    translate.path = $${PREFIX}/share/gcodeworkshop/lang

    ICONS_BASE_PATH = $$PROJECT_ROOT_PATH/install/linux/images
    icons.base = $$ICONS_BASE_PATH
    icons.files = $$findFiles($$ICONS_BASE_PATH, *.png)
    icons.path = $${PREFIX}/share/icons

    INSTALLS += translate mime desktop icons examples doc
}

macx {
}

win32 {
    examples.path = $${PREFIX}/examples
    doc.path = $${PREFIX}/doc
    translate.path = $${PREFIX}/lang

    INSTALLS += translate examples doc
}

!defined(VERSION, var): VERSION = $$getVersion()
message(Project version: $$VERSION)
