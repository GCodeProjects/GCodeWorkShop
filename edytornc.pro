
TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS = 3rdparty src-common src


include(common.pri)

QMAKE_EXTRA_TARGETS += lupdate lrelease

lupdate.commands = lupdate edytornc.pro
lrelease.target = FORCE
lrelease.commands = lrelease $$LANG_PATH/*.ts

translate.files = $$LANG_PATH/*.qm
examples.files = $$PROJECT_ROOT_PATH/examples/*
doc.files = $$PROJECT_ROOT_PATH/README.md


# target platforms dependencies
#######################################

unix {
    mime.files = $$PROJECT_ROOT_PATH/install/linux/application-x-g-code.xml
    desktop.files = $$PROJECT_ROOT_PATH/install/linux/edytornc.desktop

    examples.path = $${PREFIX}/share/edytornc/examples
    doc.path = $${PREFIX}/share/doc/edytornc
    mime.path = $${PREFIX}/share/mime/packages
    desktop.path = $${PREFIX}/share/applications
    translate.path = $${PREFIX}/share/edytornc/lang

    ICONS_BASE_PATH = $$PROJECT_ROOT_PATH/install/linux/images
    icons.base = $$ICONS_BASE_PATH
    icons.files = $$findFiles($$ICONS_BASE_PATH, *.png)
    icons.path = $${PREFIX}/share/icons

    INSTALLS += translate mime desktop icons examples doc
}

macx {
}

win32 {
}
