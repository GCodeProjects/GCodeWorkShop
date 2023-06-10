; NSIS script file

!include qt.nsi


;==============================;
; MINGW section                ;
;==============================;

Section "-Install dependies"
    SetOutPath "$INSTDIR\${DST_BINS}"

    File /oname=libgcc_s_dw2-1.dll "${CXX_BINS}\libgcc_s_dw2-1.dll"
    File /oname=libstdc++-6.dll "${CXX_BINS}\libstdc++-6.dll"
    File /oname=libwinpthread-1.dll "${CXX_BINS}\libwinpthread-1.dll"

!if /FileExists "${QT_DEP_BINS}\libEGL.dll"
    ; MINGW from Qt
    File /oname=libEGL.dll "${QT_DEP_BINS}\libEGL.dll"
    File /oname=libGLESv2.dll "${QT_DEP_BINS}\libGLESv2.dll"
!else
    ; MSYS2
    File /oname=libbrotlicommon.dll "${QT_DEP_BINS}\libbrotlicommon.dll"
    File /oname=libbrotlidec.dll "${QT_DEP_BINS}\libbrotlidec.dll"
    File /oname=libbz2-1.dll "${QT_DEP_BINS}\libbz2-1.dll"
    File /oname=libdouble-conversion.dll "${QT_DEP_BINS}\libdouble-conversion.dll"
    File /oname=libfreetype-6.dll "${QT_DEP_BINS}\libfreetype-6.dll"
    File /oname=libglib-2.0-0.dll "${QT_DEP_BINS}\libglib-2.0-0.dll"
    File /oname=libgraphite2.dll "${QT_DEP_BINS}\libgraphite2.dll"
    File /oname=libharfbuzz-0.dll "${QT_DEP_BINS}\libharfbuzz-0.dll"
    File /oname=libiconv-2.dll "${QT_DEP_BINS}\libiconv-2.dll"
    File /oname=libicudt74.dll "${QT_DEP_BINS}\libicudt74.dll"
    File /oname=libicuin74.dll "${QT_DEP_BINS}\libicuin74.dll"
    File /oname=libicuio74.dll "${QT_DEP_BINS}\libicuio74.dll"
    File /oname=libicutest74.dll "${QT_DEP_BINS}\libicutest74.dll"
    File /oname=libicutu74.dll "${QT_DEP_BINS}\libicutu74.dll"
    File /oname=libicuuc74.dll "${QT_DEP_BINS}\libicuuc74.dll"
    File /oname=libintl-8.dll "${QT_DEP_BINS}\libintl-8.dll"
    File /oname=libmd4c.dll "${QT_DEP_BINS}\libmd4c.dll"
    File /oname=libpcre2-16-0.dll "${QT_DEP_BINS}\libpcre2-16-0.dll"
    File /oname=libpcre2-8-0.dll "${QT_DEP_BINS}\libpcre2-8-0.dll"
    File /oname=libpng16-16.dll "${QT_DEP_BINS}\libpng16-16.dll"
    File /oname=libzstd.dll "${QT_DEP_BINS}\libzstd.dll"
    File /oname=zlib1.dll "${QT_DEP_BINS}\zlib1.dll"
!endif
SectionEnd

Section "un.Install dependies"
    Delete "$INSTDIR\${DST_BINS}\libgcc_s_dw2-1.dll"
    Delete "$INSTDIR\${DST_BINS}\libstdc++-6.dll"
    Delete "$INSTDIR\${DST_BINS}\libwinpthread-1.dll"

!if /FileExists "${QT_DEP_BINS}\libEGL.dll"
    ; MINGW from Qt
    Delete "$INSTDIR\${DST_BINS}\libEGL.dll"
    Delete "$INSTDIR\${DST_BINS}\libGLESv2.dll"
!else
    ; MSYS2
    Delete "$INSTDIR\${DST_BINS}\libbrotlicommon.dll"
    Delete "$INSTDIR\${DST_BINS}\libbrotlidec.dll"
    Delete "$INSTDIR\${DST_BINS}\libbz2-1.dll"
    Delete "$INSTDIR\${DST_BINS}\libdouble-conversion.dll"
    Delete "$INSTDIR\${DST_BINS}\libfreetype-6.dll"
    Delete "$INSTDIR\${DST_BINS}\libglib-2.0-0.dll"
    Delete "$INSTDIR\${DST_BINS}\libgraphite2.dll"
    Delete "$INSTDIR\${DST_BINS}\libharfbuzz-0.dll"
    Delete "$INSTDIR\${DST_BINS}\libiconv-2.dll"
    Delete "$INSTDIR\${DST_BINS}\libicudt74.dll"
    Delete "$INSTDIR\${DST_BINS}\libicuin74.dll"
    Delete "$INSTDIR\${DST_BINS}\libicuio74.dll"
    Delete "$INSTDIR\${DST_BINS}\libicutest74.dll"
    Delete "$INSTDIR\${DST_BINS}\libicutu74.dll"
    Delete "$INSTDIR\${DST_BINS}\libicuuc74.dll"
    Delete "$INSTDIR\${DST_BINS}\libintl-8.dll"
    Delete "$INSTDIR\${DST_BINS}\libmd4c.dll"
    Delete "$INSTDIR\${DST_BINS}\libpcre2-16-0.dll"
    Delete "$INSTDIR\${DST_BINS}\libpcre2-8-0.dll"
    Delete "$INSTDIR\${DST_BINS}\libpng16-16.dll"
    Delete "$INSTDIR\${DST_BINS}\libzstd.dll"
    Delete "$INSTDIR\${DST_BINS}\zlib1.dll"
!endif

    RMDir "$INSTDIR\${DST_BINS}"
    RMDir "$INSTDIR"
SectionEnd
