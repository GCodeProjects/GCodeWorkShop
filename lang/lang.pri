# Make target i18n
########################################

LANG_LIST = $$cat(languages.txt)

QMAKE_EXTRA_TARGETS += i18n
I18N_IN = $${PWD}
I18N_OUT = $$shadowed($${I18N_IN})

mkpath($${I18N_OUT})

for (LANG, LANG_LIST) {
    write_file($${I18N_OUT}/gcodeworkshop_$${LANG}.qm)
    translates.files += $${I18N_OUT}/gcodeworkshop_$${LANG}.qm
    QMAKE_EXTRA_TARGETS += i18n_g_$${LANG}
    i18n.depends += i18n_g_$${LANG}
    i18n_g_$${LANG}.commands = $$[QT_INSTALL_BINS]/lrelease $${I18N_IN}/gcodeworkshop_$${LANG}.ts -qm $${I18N_OUT}/gcodeworkshop_$${LANG}.qm
    write_file($${I18N_OUT}/kdiff3_$${LANG}.qm)
    translates.files += $${I18N_OUT}/kdiff3_$${LANG}.qm
    QMAKE_EXTRA_TARGETS += i18n_k_$${LANG}
    i18n.depends += i18n_k_$${LANG}
    i18n_k_$${LANG}.commands = $$[QT_INSTALL_BINS]/lrelease $${I18N_IN}/kdiff3_$${LANG}.ts -qm $${I18N_OUT}/kdiff3_$${LANG}.qm
}

translates.base = $$I18N_OUT
unix: translates.path = $${PREFIX}/share/gcodeworkshop/lang
win32: translates.path = $${PREFIX}/lang
!macx: INSTALLS += translates


# Make target lupdate
########################################

QMAKE_EXTRA_TARGETS += lupdate
lupdate.depends = lupdateGCodeWorkShop lupdateKdiff3
QMAKE_EXTRA_TARGETS += lupdateGCodeWorkShop lupdateKdiff3

for (LANG, LANG_LIST) {
    GCODEWORKSHOP_TS += $${I18N_IN}/gcodeworkshop_$${LANG}.ts
    KDIFF3_TS += $${I18N_IN}/kdiff3_$${LANG}.ts
}

lupdateGCodeWorkShop.commands = $$[QT_INSTALL_BINS]/lupdate \
    $$PROJECT_ROOT_PATH/gcodeworkshop \
    $$PROJECT_ROOT_PATH/gcodeshared \
    $$PROJECT_ROOT_PATH/gcodefileserver \
    $$PROJECT_ROOT_PATH/addons \
    -ts $$GCODEWORKSHOP_TS
lupdateKdiff3.commands = $$[QT_INSTALL_BINS]/lupdate \
    $$PROJECT_ROOT_PATH/3rdparty/kdiff3 \
    -ts $$KDIFF3_TS
