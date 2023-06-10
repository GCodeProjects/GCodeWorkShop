; NSIS script file


;==============================;
; Configuration                ;
;==============================;

!ifndef APP_ARCH
    !define APP_ARCH x86_64
!endif

!ifndef INSTALLER_FILE
    !define INSTALLER_FILE GCodeWorkShop_${APP_ARCH}_Setup.exe
!endif

; Base directories

!ifndef PROJECT_ROOT
    !define PROJECT_ROOT ${__FILEDIR__}\..\..\..
!endif

!ifndef BUILD_ROOT
    !if /FileExists "${BUILD_ROOT}\build\"
        !define BUILD_ROOT ${PROJECT_ROOT}\build
    !else
        !define BUILD_ROOT ${PROJECT_ROOT}
    !endif
!endif

; Sources directories

!ifndef APP_BINS
    !define APP_BINS ${PROJECT_ROOT}\bin\release
!endif

!ifndef APP_DOCS
    !define APP_DOCS ${PROJECT_ROOT}\doc
!endif

!ifndef APP_EXAMPLES
    !define APP_EXAMPLES ${PROJECT_ROOT}\examples
!endif

!ifndef APP_I18N
    !define APP_I18N ${PROJECT_ROOT}\lang
!endif

!ifndef CXX_BINS
    !ifdef CXX_RUNTIME
        !define CXX_BINS ${CXX_RUNTIME}
    !else
        !define CXX_BINS ${APP_BINS}
    !endif
!endif

!ifndef QT_BINS
    !ifdef QT_HOST_BINS
        !define QT_BINS ${QT_HOST_BINS}
    !else
        !define QT_BINS ${APP_BINS}
    !endif
!endif

!ifndef QT_DEP_BINS
    !define QT_DEP_BINS ${QT_BINS}
!endif

!ifndef QT_PLUGINS
    !ifdef QT_HOST_DATA
        !define QT_PLUGINS ${QT_HOST_DATA}\plugins
    !else
        !define QT_PLUGINS ${QT_BINS}
    !endif
!endif

!ifndef QT_I18NS
    !ifdef QT_HOST_DATA
        !define QT_I18NS ${QT_HOST_DATA}\translations
    !else
        !define QT_I18NS ${QT_BINS}\translations
    !endif
!endif

; Destination subdirectories

!ifndef DST_BINS
    !define DST_BINS bin
!endif

!ifndef DST_DOCS
    !define DST_DOCS doc
!endif

!ifndef DST_EXAMPLES
    !define DST_EXAMPLES examples
!endif

!ifndef DST_I18NS
    !define DST_I18NS lang
!endif


;==============================;
; General                      ;
;==============================;

; The icon for the installer and uninstaller.
;   MUI_ICON icon_file
;   MUI_UNICON icon_file

    Unicode true
    Name "GCodeWorkShop"
    OutFile "${INSTALLER_FILE}"

!if ${APP_ARCH} == "x86_64"
    InstallDir "$PROGRAMFILES64\GCodeWorkShop"
!else
    InstallDir "$PROGRAMFILES32\GCodeWorkShop"
!endif
    
    InstallDirRegKey HKEY_LOCAL_MACHINE "SOFTWARE\GCodeWorkShop" ""
;   SetCompressor lzma


; Modern UI

    !include "MUI.nsh"
    !define MUI_ABORTWARNING


;==============================;
; Pages                        ;
;==============================;
 
    Var STARTMENU_FOLDER

    !insertmacro MUI_PAGE_WELCOME
    !insertmacro MUI_PAGE_LICENSE "${PROJECT_ROOT}\LICENSE"
    !insertmacro MUI_PAGE_DIRECTORY

    ;Start Menu Folder Page Configuration
    !define MUI_STARTMENUPAGE_REGISTRY_ROOT HKEY_LOCAL_MACHINE
    !define MUI_STARTMENUPAGE_REGISTRY_KEY "SOFTWARE\GCodeWorkShop"
    !define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "Start Menu Folder"
    !insertmacro MUI_PAGE_STARTMENU Application $STARTMENU_FOLDER

    !insertmacro MUI_PAGE_INSTFILES
    !insertmacro MUI_PAGE_FINISH

    ; Uninstaller pages
    !insertmacro MUI_UNPAGE_WELCOME
    !insertmacro MUI_UNPAGE_CONFIRM
    !insertmacro MUI_UNPAGE_INSTFILES
    !insertmacro MUI_UNPAGE_FINISH


;==============================;
; Languages                    ;
;==============================;

    !insertmacro MUI_LANGUAGE "English"
    !insertmacro MUI_LANGUAGE "Catalan"
    !insertmacro MUI_LANGUAGE "Czech"
    !insertmacro MUI_LANGUAGE "Dutch"
    !insertmacro MUI_LANGUAGE "Finnish"
    !insertmacro MUI_LANGUAGE "German"
    !insertmacro MUI_LANGUAGE "Polish"
    !insertmacro MUI_LANGUAGE "Russian"
    !insertmacro MUI_LANGUAGE "Spanish"


;==============================;
; Main section                 ;
;==============================;

Section "-Install"
    SetOutPath "$INSTDIR"
    File /oname=LICENSE.txt "${PROJECT_ROOT}\LICENSE"

    SetOutPath "$INSTDIR\${DST_BINS}"
    File /oname=GCodeWorkShop.exe "${APP_BINS}\gcodeworkshop.exe"
    File /oname=GCodeFileServer.exe "${APP_BINS}\gcodefileserver.exe"
    File /oname=qt.conf "${__FILEDIR__}\..\qt.conf"

    ;Store installation folder
    WriteRegStr HKEY_LOCAL_MACHINE "SOFTWARE\GCodeWorkShop" "" "$INSTDIR"

    ;Create uninstaller
    WriteUninstaller "$INSTDIR\uninst.exe"

    !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
    ;Create shortcuts
    CreateDirectory "$SMPROGRAMS\$STARTMENU_FOLDER"
    CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\GCodeWorkShop.lnk" "$INSTDIR\${DST_BINS}\GCodeWorkShop.exe"
    CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\Uninstall.lnk" "$INSTDIR\uninst.exe"
    !insertmacro MUI_STARTMENU_WRITE_END
SectionEnd

Section "Uninstall"
    Push $R0
    !insertmacro MUI_STARTMENU_GETFOLDER Application $R0
    Delete "$SMPROGRAMS\$R0\Uninstall.lnk"
    Delete "$SMPROGRAMS\$R0\GCodeWorkShop.lnk"
    RMDir  "$SMPROGRAMS\$R0"
    Pop $R0

    DeleteRegKey HKEY_LOCAL_MACHINE "SOFTWARE\GCodeWorkShop"
    DeleteRegKey HKEY_LOCAL_MACHINE "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\GCodeWorkShop"

    Delete "$INSTDIR\${DST_BINS}\GCodeWorkShop.exe"
    Delete "$INSTDIR\${DST_BINS}\GCodeFileServer.exe"
    Delete "$INSTDIR\${DST_BINS}\qt.conf"
    Delete "$INSTDIR\LICENSE.txt"
    Delete "$INSTDIR\uninst.exe"

    RMDir "$INSTDIR\${DST_BINS}"
    RMDir "$INSTDIR"
SectionEnd


;==============================;
; Internationalisation         ;
;==============================;

Section "-Install i18n"
    SetOutPath "$INSTDIR\${DST_I18NS}"

    File /oname=gcodeworkshop_ca.qm "${APP_I18N}\gcodeworkshop_ca.qm" 
    File /oname=gcodeworkshop_cs_CZ.qm "${APP_I18N}\gcodeworkshop_cs_CZ.qm"
    File /oname=gcodeworkshop_de.qm "${APP_I18N}\gcodeworkshop_de.qm"
    File /oname=gcodeworkshop_es.qm "${APP_I18N}\gcodeworkshop_es.qm"
    File /oname=gcodeworkshop_fi.qm "${APP_I18N}\gcodeworkshop_fi.qm"
    File /oname=gcodeworkshop_nl.qm "${APP_I18N}\gcodeworkshop_nl.qm"
    File /oname=gcodeworkshop_pl.qm "${APP_I18N}\gcodeworkshop_pl.qm"
    File /oname=gcodeworkshop_ru.qm "${APP_I18N}\gcodeworkshop_ru.qm"
    File /oname=kdiff3_ca.qm "${APP_I18N}\kdiff3_ca.qm"
    File /oname=kdiff3_cs_CZ.qm "${APP_I18N}\kdiff3_cs_CZ.qm"
    File /oname=kdiff3_de.qm "${APP_I18N}\kdiff3_de.qm"
    File /oname=kdiff3_es.qm "${APP_I18N}\kdiff3_es.qm"
    File /oname=kdiff3_fi.qm "${APP_I18N}\kdiff3_fi.qm"
    File /oname=kdiff3_nl.qm "${APP_I18N}\kdiff3_nl.qm"
    File /oname=kdiff3_pl.qm "${APP_I18N}\kdiff3_pl.qm"
    File /oname=kdiff3_ru.qm "${APP_I18N}\kdiff3_ru.qm"
SectionEnd

Section "un.Install i18n"
    Delete "$INSTDIR\${DST_I18NS}\gcodeworkshop_ca.qm" 
    Delete "$INSTDIR\${DST_I18NS}\gcodeworkshop_cs_CZ.qm"
    Delete "$INSTDIR\${DST_I18NS}\gcodeworkshop_de.qm"
    Delete "$INSTDIR\${DST_I18NS}\gcodeworkshop_es.qm"
    Delete "$INSTDIR\${DST_I18NS}\gcodeworkshop_fi.qm"
    Delete "$INSTDIR\${DST_I18NS}\gcodeworkshop_nl.qm"
    Delete "$INSTDIR\${DST_I18NS}\gcodeworkshop_pl.qm"
    Delete "$INSTDIR\${DST_I18NS}\gcodeworkshop_ru.qm"
    Delete "$INSTDIR\${DST_I18NS}\kdiff3_ca.qm"
    Delete "$INSTDIR\${DST_I18NS}\kdiff3_cs_CZ.qm"
    Delete "$INSTDIR\${DST_I18NS}\kdiff3_de.qm"
    Delete "$INSTDIR\${DST_I18NS}\kdiff3_es.qm"
    Delete "$INSTDIR\${DST_I18NS}\kdiff3_fi.qm"
    Delete "$INSTDIR\${DST_I18NS}\kdiff3_nl.qm"
    Delete "$INSTDIR\${DST_I18NS}\kdiff3_pl.qm"
    Delete "$INSTDIR\${DST_I18NS}\kdiff3_ru.qm"

    RMDir "$INSTDIR\${DST_I18NS}"
    RMDir "$INSTDIR"
SectionEnd


;==============================;
; Documentation                ;
;==============================;

Section "-Install doc"
    SetOutPath "$INSTDIR\${DST_DOCS}"

    File /oname=SerialTransmission_Help.html "${APP_DOCS}\SerialTransmission_Help.html"
SectionEnd

Section "un.Install doc"
    Delete "$INSTDIR\${DST_DOCS}\SerialTransmission_Help.html"

    RMDir "$INSTDIR\${DST_DOCS}"
    RMDir "$INSTDIR"
SectionEnd


;==============================;
; Examples                     ;
;==============================;

Section "-Install Examples"
    SetOutPath "$INSTDIR\${DST_EXAMPLES}"

    File /oname=0100.nc "${APP_EXAMPLES}\0100.nc"
    File /oname=FANUC0M.nc "${APP_EXAMPLES}\FANUC0M.nc"
    File /oname=FANUC0T.nc "${APP_EXAMPLES}\FANUC0T.nc"
    File /oname=FLANGE.nc "${APP_EXAMPLES}\FLANGE.nc"
    File /oname=HAAS.nc "${APP_EXAMPLES}\HAAS.nc"
    File /oname=HK.nc "${APP_EXAMPLES}\HK.nc"
    File /oname=OSP5020M.nc "${APP_EXAMPLES}\OSP5020M.nc"
    File /oname=OSP7000L.nc "${APP_EXAMPLES}\OSP7000L.nc"
    File /oname=PHILIPS.nc "${APP_EXAMPLES}\PHILIPS.nc"
    File /oname=SINUMERIK840D.nc "${APP_EXAMPLES}\SINUMERIK840D.nc"
    File /oname=SINUMERIK850.nc "${APP_EXAMPLES}\SINUMERIK850.nc"
    File /oname=TOKARKA.nc "${APP_EXAMPLES}\TOKARKA.NC"
    File /oname=cnc_tips.txt "${APP_EXAMPLES}\cnc_tips.txt"
    File /oname=macro7.nc "${APP_EXAMPLES}\macro7.nc"
SectionEnd

Section "un.Install Examples"
    Delete "$INSTDIR\${DST_EXAMPLES}\0100.nc"
    Delete "$INSTDIR\${DST_EXAMPLES}\FANUC0M.nc"
    Delete "$INSTDIR\${DST_EXAMPLES}\FANUC0T.nc"
    Delete "$INSTDIR\${DST_EXAMPLES}\FLANGE.nc"
    Delete "$INSTDIR\${DST_EXAMPLES}\HAAS.nc"
    Delete "$INSTDIR\${DST_EXAMPLES}\HK.nc"
    Delete "$INSTDIR\${DST_EXAMPLES}\OSP5020M.nc"
    Delete "$INSTDIR\${DST_EXAMPLES}\OSP7000L.nc"
    Delete "$INSTDIR\${DST_EXAMPLES}\PHILIPS.nc"
    Delete "$INSTDIR\${DST_EXAMPLES}\SINUMERIK840D.nc"
    Delete "$INSTDIR\${DST_EXAMPLES}\SINUMERIK850.nc"
    Delete "$INSTDIR\${DST_EXAMPLES}\TOKARKA.nc"
    Delete "$INSTDIR\${DST_EXAMPLES}\cnc_tips.txt"
    Delete "$INSTDIR\${DST_EXAMPLES}\macro7.nc"

    RMDir "$INSTDIR\${DST_EXAMPLES}"
    RMDir "$INSTDIR"
SectionEnd
