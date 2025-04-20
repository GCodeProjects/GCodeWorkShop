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

; Directories

!define APP_BIN_DIR bin
!define PKG_BIN_DIR ${APP_BIN_DIR}
!define APP_DOC_DIR doc
!define PKG_DOC_DIR  ${APP_DOC_DIR}
!define APP_EXAMPLES_DIR examples
!define PKG_EXAMPLES_DIR ${APP_EXAMPLES_DIR}
!define APP_I18N_DIR lang
!define PKG_I18N_DIR ${APP_I18N_DIR}


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
    !insertmacro MUI_PAGE_LICENSE LICENSE.txt
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
    File LICENSE.txt
    File LICENSE.LESSER.txt

    SetOutPath "$INSTDIR\${PKG_BIN_DIR}"
    File /oname=GCodeWorkShop.exe ${APP_BIN_DIR}\gcodeworkshop.exe
    File /oname=GCodeFileServer.exe ${APP_BIN_DIR}\gcodefileserver.exe

    ;Store installation folder
    WriteRegStr HKEY_LOCAL_MACHINE "SOFTWARE\GCodeWorkShop" "" "$INSTDIR"

    ;Create uninstaller
    WriteUninstaller "$INSTDIR\uninst.exe"

    !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
    ;Create shortcuts
    CreateDirectory "$SMPROGRAMS\$STARTMENU_FOLDER"
    CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\GCodeWorkShop.lnk" "$INSTDIR\${PKG_BIN_DIR}\GCodeWorkShop.exe"
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

    Delete "$INSTDIR\${PKG_BIN_DIR}\GCodeWorkShop.exe"
    Delete "$INSTDIR\${PKG_BIN_DIR}\GCodeFileServer.exe"
    Delete "$INSTDIR\LICENSE.txt"
    Delete "$INSTDIR\LICENSE.LESSER.txt"
    Delete "$INSTDIR\uninst.exe"

    RMDir "$INSTDIR\${PKG_BIN_DIR}"
    RMDir "$INSTDIR"
SectionEnd


;==============================;
; Internationalisation         ;
;==============================;

Section "-Install i18n"
    SetOutPath "$INSTDIR\${PKG_I18N_DIR}"
    File "${APP_I18N_DIR}\*.qm"
SectionEnd

Section "un.Install i18n"
    RMDir /r "$INSTDIR\${PKG_I18N_DIR}"
    RMDir "$INSTDIR"
SectionEnd


;==============================;
; Documentation                ;
;==============================;

Section "-Install doc"
    SetOutPath "$INSTDIR\${PKG_DOC_DIR}"
    File /r "${APP_DOC_DIR}\*.html"
SectionEnd

Section "un.Install doc"
    RMDir /r "$INSTDIR\${PKG_DOC_DIR}"
    RMDir "$INSTDIR"
SectionEnd


;==============================;
; Examples                     ;
;==============================;

Section "-Install Examples"
    SetOutPath "$INSTDIR\${PKG_EXAMPLES_DIR}"
    File /r "${APP_EXAMPLES_DIR}\*.nc"
    File "${APP_EXAMPLES_DIR}\cnc_tips.txt"
SectionEnd

Section "un.Install Examples"
    RMDir /r "$INSTDIR\${PKG_EXAMPLES_DIR}"
    RMDir "$INSTDIR"
SectionEnd


;==============================;
; dependencies                    ;
;==============================;

Section "-Install Dependencies"
    SetOutPath "$INSTDIR\${PKG_BIN_DIR}"
    File ${APP_BIN_DIR}\qt.conf
    File /nonfatal /r "${APP_BIN_DIR}\*.dll"
SectionEnd

Section "un.Install Dependencies"
    RMDir /r "$INSTDIR\${PKG_BIN_DIR}"
    RMDir "$INSTDIR"
SectionEnd
