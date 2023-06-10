; NSIS script file


;==============================;
; Qt section                   ;
;==============================;

Section "-Install Qt"
    SetOutPath "$INSTDIR\${DST_BINS}"

    File /oname=Qt5Core.dll "${QT_BINS}\Qt5Core.dll"
    File /oname=Qt5Gui.dll "${QT_BINS}\Qt5Gui.dll"
    File /oname=Qt5Network.dll "${QT_BINS}\Qt5Network.dll"
    File /oname=Qt5PrintSupport.dll "${QT_BINS}\Qt5PrintSupport.dll"
    File /oname=Qt5SerialPort.dll "${QT_BINS}\Qt5SerialPort.dll"
    File /oname=Qt5Widgets.dll "${QT_BINS}\Qt5Widgets.dll"

    CreateDirectory "$INSTDIR\${DST_BINS}\bearer"
    CreateDirectory "$INSTDIR\${DST_BINS}\imageformats"
    CreateDirectory "$INSTDIR\${DST_BINS}\platforms"
    CreateDirectory "$INSTDIR\${DST_BINS}\printsupport"
    CreateDirectory "$INSTDIR\${DST_BINS}\styles"

    ; Qt plugins
    File /oname=bearer\qgenericbearer.dll "${QT_PLUGINS}\bearer\qgenericbearer.dll"
    File /oname=imageformats\qgif.dll "${QT_PLUGINS}\imageformats\qgif.dll"
    File /oname=imageformats\qico.dll "${QT_PLUGINS}\imageformats\qico.dll"
    File /oname=imageformats\qjpeg.dll "${QT_PLUGINS}\imageformats\qjpeg.dll"
    File /oname=platforms\qwindows.dll "${QT_PLUGINS}\platforms\qwindows.dll"
    File /oname=printsupport\windowsprintersupport.dll "${QT_PLUGINS}\printsupport\windowsprintersupport.dll"
    File /oname=styles\qwindowsvistastyle.dll "${QT_PLUGINS}\styles\qwindowsvistastyle.dll"

    CreateDirectory "$INSTDIR\${DST_BINS}\translations"
    File /nonfatal /oname=translations\qt_ca.qm "${QT_I18NS}\qt_ca.qm"
    File /nonfatal /oname=translations\qtbase_ca.qm "${QT_I18NS}\qtbase_ca.qm"
    File /nonfatal /oname=translations\qtserialport_ca.qm "${QT_I18NS}\qtserialport_ca.qm"
    File /nonfatal /oname=translations\qt_cs.qm "${QT_I18NS}\qt_cs.qm"
    File /nonfatal /oname=translations\qtbase_cs.qm "${QT_I18NS}\qtbase_cs.qm"
    File /nonfatal /oname=translations\qtserialport_cs.qm "${QT_I18NS}\qtserialport_cs.qm"
    File /nonfatal /oname=translations\qt_de.qm "${QT_I18NS}\qt_de.qm"
    File /nonfatal /oname=translations\qtbase_de.qm "${QT_I18NS}\qtbase_de.qm"
    File /nonfatal /oname=translations\qtserialport_de.qm "${QT_I18NS}\qtserialport_de.qm"
    File /nonfatal /oname=translations\qt_es.qm "${QT_I18NS}\qt_es.qm"
    File /nonfatal /oname=translations\qtbase_es.qm "${QT_I18NS}\qtbase_es.qm"
    File /nonfatal /oname=translations\qtserialport_es.qm "${QT_I18NS}\qtserialport_es.qm"
    File /nonfatal /oname=translations\qt_fi.qm "${QT_I18NS}\qt_fi.qm"
    File /nonfatal /oname=translations\qtbase_fi.qm "${QT_I18NS}\qtbase_fi.qm"
    File /nonfatal /oname=translations\qtserialport_fi.qm "${QT_I18NS}\qtserialport_fi.qm"
    File /nonfatal /oname=translations\qt_nl.qm "${QT_I18NS}\qt_nl.qm"
    File /nonfatal /oname=translations\qtbase_nl.qm "${QT_I18NS}\qtbase_nl.qm"
    File /nonfatal /oname=translations\qtserialport_nl.qm "${QT_I18NS}\qtserialport_nl.qm"
    File /nonfatal /oname=translations\qt_pl.qm "${QT_I18NS}\qt_pl.qm"
    File /nonfatal /oname=translations\qtbase_pl.qm "${QT_I18NS}\qtbase_pl.qm"
    File /nonfatal /oname=translations\qtserialport_pl.qm "${QT_I18NS}\qtserialport_pl.qm"
    File /nonfatal /oname=translations\qt_ru.qm "${QT_I18NS}\qt_ru.qm"
    File /nonfatal /oname=translations\qtbase_ru.qm "${QT_I18NS}\qtbase_ru.qm"
    File /nonfatal /oname=translations\qtserialport_ru.qm "${QT_I18NS}\qtserialport_ru.qm"
SectionEnd

Section "un.Install Qt"
    Delete "$INSTDIR\${DST_BINS}\Qt5Core.dll"
    Delete "$INSTDIR\${DST_BINS}\Qt5Gui.dll"
    Delete "$INSTDIR\${DST_BINS}\Qt5Network.dll"
    Delete "$INSTDIR\${DST_BINS}\Qt5PrintSupport.dll"
    Delete "$INSTDIR\${DST_BINS}\Qt5SerialPort.dll"
    Delete "$INSTDIR\${DST_BINS}\Qt5Widgets.dll"

    ; Qt plugins
    Delete "$INSTDIR\${DST_BINS}\bearer\qgenericbearer.dll"
    Delete "$INSTDIR\${DST_BINS}\imageformats\qgif.dll"
    Delete "$INSTDIR\${DST_BINS}\imageformats\qico.dll"
    Delete "$INSTDIR\${DST_BINS}\imageformats\qjpeg.dll"
    Delete "$INSTDIR\${DST_BINS}\platforms\qwindows.dll"
    Delete "$INSTDIR\${DST_BINS}\printsupport\windowsprintersupport.dll"
    Delete "$INSTDIR\${DST_BINS}\styles\qwindowsvistastyle.dll"

    Delete "$INSTDIR\${DST_BINS}\translations\qt_*.qm"
    Delete "$INSTDIR\${DST_BINS}\translations\qtbase_*.qm"
    Delete "$INSTDIR\${DST_BINS}\translations\qtserialport_*.qm"

    RMDir "$INSTDIR\${DST_BINS}\bearer"
    RMDir "$INSTDIR\${DST_BINS}\imageformats"
    RMDir "$INSTDIR\${DST_BINS}\platforms"
    RMDir "$INSTDIR\${DST_BINS}\printsupport"
    RMDir "$INSTDIR\${DST_BINS}\styles"
    RMDir "$INSTDIR\${DST_BINS}\translations"
    RMDir "$INSTDIR\${DST_BINS}"
    RMDir "$INSTDIR"
SectionEnd
