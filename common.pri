# http://itnotesblog.ru/note.php?id=3


PROJECT_ROOT_PATH = $${PWD}/

OS_SUFFIX = unc
win32:  OS_SUFFIX = win
linux*: OS_SUFFIX = linux
macx:   OS_SUFFIX = macx

OS_SUFFIX = $${OS_SUFFIX}.$${QMAKE_HOST.arch}

CONFIG(debug, debug|release) {
    BUILD_FLAG = debug
} else {
    BUILD_FLAG = release
    # Supress debug message
    DEFINES *= QT_NO_DEBUG_OUTPUT # QT_NO_INFO_OUTPUT
}

BUILD_FLAG = $${OS_SUFFIX}.$${BUILD_FLAG}

CONFIG *= c++11 warn_on
CONFIG -= debug_and_release debug_and_release_target

# moc doesn't detect Q_OS_LINUX correctly, so add this to make it work
linux*:DEFINES *= __linux__

LANG_PATH = $${PROJECT_ROOT_PATH}/lang/
IMPORT_PATH = $${PROJECT_ROOT_PATH}/3rdparty/
IMPORT_LIBS_PATH = $${IMPORT_PATH}/lib/$${BUILD_FLAG}/
BIN_PATH = $${PROJECT_ROOT_PATH}/bin/$${BUILD_FLAG}/

BUILD_PATH = $${PROJECT_ROOT_PATH}/build/$${BUILD_FLAG}/$${TARGET}/
RCC_DIR = $${BUILD_PATH}/rcc/
UI_DIR = $${BUILD_PATH}/ui/
MOC_DIR = $${BUILD_PATH}/moc/
OBJECTS_DIR = $${BUILD_PATH}/obj/

DESTDIR = $${BIN_PATH}/
CONFIG(staticlib) {
    DESTDIR = $${IMPORT_LIBS_PATH}/
}

LIBS += -L$${IMPORT_LIBS_PATH}/
INCLUDEPATH += $${PROJECT_ROOT_PATH}/include/
INCLUDEPATH += $${IMPORT_PATH}/
INCLUDEPATH += $${IMPORT_PATH}/include/


#
# function findFiles(dir, basename)
# Returns a list of files with this basename.
# The function searches for dir and subdir recursively.
#
defineReplace(findFiles) {
    dir = $$1
    basemane = $$2

    file_list = $$files($$dir/$$basemane)
    path_list = $$files($$dir/*)

    for(path, path_list) {
        file_list += $$findFiles($$path, $$basemane)
    }

    return ($$file_list)
}
