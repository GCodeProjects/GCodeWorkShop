
!defined(INSTALLER_DIR, var):INSTALLER_DIR = $$shadowed($${PROJECT_ROOT_PATH})

win32:include(windows/windows.pri)
