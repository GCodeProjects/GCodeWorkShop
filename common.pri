

PROJECT_ROOT_PATH = $${PWD}
PROJECT_BUILD_PATH = $$shadowed($$PROJECT_ROOT_PATH)

CONFIG(debug, debug|release) {
    BUILD_FLAG = debug
    CONFIG *= warn_on
} else {
    BUILD_FLAG = release
    # Supress debug message
    DEFINES *= QT_NO_DEBUG_OUTPUT # QT_NO_INFO_OUTPUT
}

CONFIG *= c++11
CONFIG -= debug_and_release debug_and_release_target

# moc doesn't detect Q_OS_LINUX correctly, so add this to make it work
linux*:DEFINES *= __linux__

LANG_PATH = $${PROJECT_ROOT_PATH}/lang
# define locales for translation
LANG_LIST = pl ca de fi cs_CZ es nl ru

LIBS_PATH = $${PROJECT_BUILD_PATH}/libs
BIN_PATH = $${PROJECT_ROOT_PATH}/bin/$${BUILD_FLAG}

DESTDIR = $${BIN_PATH}
CONFIG(staticlib) {
    DESTDIR = $$LIBS_PATH
}

LIBS += -L$${LIBS_PATH} -L$${BIN_PATH}
INCLUDEPATH += $${PROJECT_ROOT_PATH}/include
INCLUDEPATH += $${PROJECT_ROOT_PATH}/3rdparty
INCLUDEPATH += $${PROJECT_ROOT_PATH}/3rdparty/include

!defined(PREFIX, var) {
    unix:PREFIX = /usr/local/
    win32:PREFIX = $$(programfiles)
}


#
# function findFiles(dir, basename)
# Returns a list of files with this basename.
# The function searches for dir and subdir recursively.
#
defineReplace(findFiles) {
    dir = $$1
    base_name = $$2

    file_list = $$files($$dir/$$base_name)
    path_list = $$files($$dir/*)

    for(path, path_list) {
        file_list += $$findFiles($$path, $$base_name)
    }

    return ($$file_list)
}


#
# function tsFiles(modulename, locales)
# Returns a list of files with this $${LANG_PATH}/modulename_lang.ts
# example :
# TARGET = bar
# LANG_PATH = lang
# LANG_LIST = pl fr de
# $$tsFiles(foo, ru en) returns lang/foo_ru.ts lang/foo_en.ts
# $$tsFiles(foo) returns lang/foo_pl.ts lang/foo_fr.ts lang/foo_de.ts
# $$tsFiles() returns lang/bar_pl.ts lang/bar_fr.ts lang/bar_de.ts
#
defineReplace(tsFiles) {
    module_name = $$1
    locales = $$2

    !defined($$1, var) {
        module_name = $$TARGET
    }

    !defined($$2, var) {
        locales = $$LANG_LIST
    }

    for(lang, locales) {
        file_list += $${LANG_PATH}/$${module_name}_$${lang}.ts
    }

    return ($$file_list)
}
