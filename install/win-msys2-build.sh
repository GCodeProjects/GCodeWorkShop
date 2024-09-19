#!/usr/bin/env bash

set -e

function usage()
{
  echo  -e "\
Build the application and create the installer.\n\
Usage:\n\
 $(basename "$0") [-b bld_dir] [-p pkg_dir] [-q qmake] [-s suffix] [-v version]\n\
\n\
Options:\n\
  -b bld_dir  The directory where the application will be built.\n\
              By default it is <source_root>/build\n\
  -p pkg_dir  The directory where the installer will be built.\n\
              By default this is <bld_dir>/<pkg_name>_<pkg_version>\n\
  -q qmake    The name or full path of the qmake utility. This can be useful\n\
              when building with static linking or when building with Qt6.\n\
  -s suffux   The suffix that will be added to the installer name after the\n\
              version and before the architecture type.\n\
              Nothing is added by default.\n\
  -v version  Define the installer version. By default version gettings from\n\
              the version.h file.\n\
\n\
It is assumed that the necessary dependencies and tools are already installed\n\
in the system.\n\
\n\
The created installer is saved in the current working directory.\
" 1>&2
}

MAKE_JOBS=$(nproc)
MODE_FORCE_CLEAN=1
#MODE_QUIET=1

SCRIPT_DIR=$(realpath "$(dirname "$0")")

# The package configuration must define the get_version function
# and the following variables:
# SOURCE_DIR
# PACKAGE_NAME
source "${SCRIPT_DIR}"/package.cfg

function echo_step()
{
    [ -v MODE_QUIET ] && return
    echo -e "\n================================================================================"
    echo -e "$1"
    echo -e "================================================================================\n"
}

QMAKE=qmake
BUILD_DIR=${SOURCE_DIR}/build
PACKAGE_VERSION=$(get_version)

if [ "$MSYSTEM" == MINGW32 ]; then
    PACKAGE_ARCH=i686
else
    PACKAGE_ARCH=x86_64
fi

while getopts ":b:p:q:s:v:h" options; do
    case "${options}" in
    b)
        BUILD_DIR=${OPTARG}
        ;;
    p)
        PACKAGE_DIR=${OPTARG}
        FORCE_PACKAGE_DIR=1
        ;;
    q)
        QMAKE=${OPTARG}
        ;;
    s)
        PACKAGE_SUFFIX=_${OPTARG}
        ;;
    v)
        PACKAGE_VERSION=${OPTARG}
        FORCE_VERSION=1
        ;;
    h)
        usage
        exit 0
        ;;
    *)
        echo -e "Error: unknown option '-${OPTARG}'.\n"
        usage
        exit 1
        ;;
    esac
done

if ! [ -v FORCE_PACKAGE_DIR ]; then
    PACKAGE_DIR=${BUILD_DIR}/${PACKAGE_NAME}_${PACKAGE_VERSION}
fi
PACKAGE_BIN_DIR=${PACKAGE_DIR}/bin
PACKAGE_I18N_DIR=${PACKAGE_DIR}/lang
PACKAGE_FULL_NAME=${PACKAGE_NAME}-${PACKAGE_VERSION}${PACKAGE_SUFFIX}_${PACKAGE_ARCH}

echo_step "Try to build package with:\n\
PACKAGE_NAME      ${PACKAGE_NAME}\n\
PACKAGE_SUFFIX    ${PACKAGE_SUFFIX}\n\
PACKAGE_VERSION   ${PACKAGE_VERSION}\n\
PACKAGE_ARCH      ${PACKAGE_ARCH}\n\
PACKAGE_FULL_NAME ${PACKAGE_FULL_NAME}\n\
SOURCE_DIR        ${SOURCE_DIR}\n\
BUILD_DIR         ${BUILD_DIR}\n\
PACKAGE_DIR       ${PACKAGE_DIR}\n\
"

if [ -v MODE_FORCE_CLEAN ]; then
    echo_step "Clean the build tree and the package dir"
    if [ -d  "$BUILD_DIR" ]; then
        echo "Remove $BUILD_DIR"
        rm -rf "$BUILD_DIR"
    fi
    if [ -d "$PACKAGE_DIR" ]; then
        echo "Remove $PACKAGE_DIR"
        rm -rf "$PACKAGE_DIR"
    fi
fi

echo_step "Run qmake"
QT_PLUGINS=$($QMAKE -query QT_INSTALL_PLUGINS)
QT_I18NS=$($QMAKE -query QT_INSTALL_TRANSLATIONS)
if [ -v FORCE_VERSION ]; then
    QMAKE_OPTIONS="VERSION=$PACKAGE_VERSION "
fi
QMAKE_OPTIONS+="PREFIX=$PACKAGE_DIR $SOURCE_DIR"
OLD_PWD=$PWD
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"
$QMAKE -r $QMAKE_OPTIONS

echo_step "Make translations"
# At the time qmake is first called, the *.qm files do not yet exist and qmake
# does not generate code to install these files. Therefore, after compiling
# the *.qm files, call qmake again to generate the installation code correctly.
make lrelease -j $MAKE_JOBS
$QMAKE $QMAKE_OPTIONS

echo_step "Make application"
make -j $MAKE_JOBS

echo_step "Copy application to the package directory"
make install
cp "$SOURCE_DIR"/LICENSE "$PACKAGE_DIR"/LICENSE.txt

mkdir -p "$PACKAGE_BIN_DIR"
cat <<EOF > "$PACKAGE_BIN_DIR"/qt.conf
; https://doc.qt.io/qt-5/qt-conf.html

[Paths]
Translations=../lang
EOF

if [ -f "${QT_PLUGINS}"/platforms/qwindows.dll ]; then
    echo "Coping Qt plugins"
    mkdir -p "$PACKAGE_BIN_DIR"/bearer
    mkdir -p "$PACKAGE_BIN_DIR"/imageformats
    mkdir -p "$PACKAGE_BIN_DIR"/platforms
    mkdir -p "$PACKAGE_BIN_DIR"/printsupport
    mkdir -p "$PACKAGE_BIN_DIR"/styles
    cp "${QT_PLUGINS}"/bearer/qgenericbearer.dll "$PACKAGE_BIN_DIR"/bearer
    cp "${QT_PLUGINS}"/imageformats/qgif.dll \
       "${QT_PLUGINS}"/imageformats/qico.dll \
       "${QT_PLUGINS}"/imageformats/qjpeg.dll "$PACKAGE_BIN_DIR"/imageformats
    cp "${QT_PLUGINS}"/platforms/qwindows.dll "$PACKAGE_BIN_DIR"/platforms
    cp "${QT_PLUGINS}"/printsupport/windowsprintersupport.dll "$PACKAGE_BIN_DIR"/printsupport
    cp "${QT_PLUGINS}"/styles/qwindowsvistastyle.dll "$PACKAGE_BIN_DIR"/styles
fi

echo "Coping Qt translations"
for lang in 'ca' 'cs' 'de' 'es' 'fi' 'nl' 'pl' 'ru'; do
    [ -f "${QT_I18NS}/qt_${lang}.qm" ] && cp "${QT_I18NS}/qt_${lang}.qm" "$PACKAGE_I18N_DIR"
    [ -f "${QT_I18NS}/qtbase_${lang}.qm" ] && cp "${QT_I18NS}/qtbase_${lang}.qm" "$PACKAGE_I18N_DIR"
    [ -f "${QT_I18NS}/qtserialport_${lang}.qm" ] && cp "${QT_I18NS}/qtserialport_${lang}.qm" "$PACKAGE_I18N_DIR"
done

echo "Coping DLL's"
for dll in $(ldd $(find "$PACKAGE_BIN_DIR" -type f -name "*.exe") | sort -u | grep mingw | awk '{print $1}'); do
    cp "$(which "$dll")" "$PACKAGE_BIN_DIR"
done

if command -v makensis >/dev/null 2>&1; then
    echo_step "Create installer"
    cp "${SCRIPT_DIR}"/win-installer.nsi "${PACKAGE_DIR}"
    if [ -f "${OLD_PWD}/${PACKAGE_FULL_NAME}".exe ]; then
        echo "Remove old installer ${OLD_PWD}/${PACKAGE_FULL_NAME}.exe"
        rm "${OLD_PWD}/${PACKAGE_FULL_NAME}".exe
    fi
    makensis \
      -DAPP_ARCH="$PACKAGE_ARCH" \
      -DINSTALLER_FILE="${OLD_PWD}/${PACKAGE_FULL_NAME}".exe \
      "${PACKAGE_DIR}"/win-installer.nsi
    rm "${PACKAGE_DIR}"/win-installer.nsi
fi

if command -v zip >/dev/null 2>&1; then
    echo_step "Create portable"
    ZIP_NAME=${PACKAGE_FULL_NAME}.zip
    if [ -f "${OLD_PWD}/$ZIP_NAME" ]; then
        echo "Remove old portable ${OLD_PWD}/$ZIP_NAME"
        rm "${OLD_PWD}/$ZIP_NAME"
    fi
    cd "${PACKAGE_DIR}/.."
    zip -r "${OLD_PWD}/$ZIP_NAME" "$(basename "${PACKAGE_DIR}")"
fi
