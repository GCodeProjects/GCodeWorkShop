@echo off
set EXECDIR=%cd%

rem TODO: Better way to reference
set WINDEPLOY="c:\Qt\5.15.2\mingw81_64\bin\windeployqt.exe"

set SOURCEDIR="%EXECDIR%\..\..\bin\release"

cd %SOURCEDIR%

"%WINDEPLOY% %SOURCEDIR%\edytornc.exe"

"%WIX%bin\candle" *.wxs -o %EXECDIR%\obj\ -ext WixUIExtension
if %ERRORLEVEL% EQU 0 (
    "%WIX%bin\light" %EXECDIR%\obj\*.wixobj -o %EXECDIR%\bin\edytornc.msi
)