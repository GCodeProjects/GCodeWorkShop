# http://itnotesblog.ru/note.php?id=3


PROJECT_ROOT_PATH = $${PWD}/

win32: OS_SUFFIX = win32
linux-g++: OS_SUFFIX = linux

CONFIG(debug, debug|release) {
    BUILD_FLAG = debug
    LIB_SUFFIX = d
} else {
    BUILD_FLAG = release
}

CONFIG += c++11 warn_on

LANG_PATH = $${PROJECT_ROOT_PATH}/lang/
IMPORT_PATH = $${PROJECT_ROOT_PATH}/3rdparty/
IMPORT_LIBS_PATH = $${IMPORT_PATH}/lib/$${OS_SUFFIX}/
BIN_PATH = $${PROJECT_ROOT_PATH}/bin/$${BUILD_FLAG}/

BUILD_PATH = $${PROJECT_ROOT_PATH}/build/$${BUILD_FLAG}/$${TARGET}/
RCC_DIR = $${BUILD_PATH}/rcc/
UI_DIR = $${BUILD_PATH}/ui/
MOC_DIR = $${BUILD_PATH}/moc/
OBJECTS_DIR = $${BUILD_PATH}/obj/

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
