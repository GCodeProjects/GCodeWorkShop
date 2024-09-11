#!/usr/bin/env bash

set -e

function usage()
{
  echo  -e "\
Build the application and create the package.\n\
Usage:\n\
 $(basename "$0") [-b bld_dir] [-p pkg_dir] [-q qmake] [-s suffix] [-v version]\n\
\n\
Options:\n\
  -b bld_dir  The directory where the application will be built.\n\
              By default it is <source_root>/build\n\
  -p pkg_dir  The directory where the package will be built.\n\
              By default this is <bld_dir>/<pkg_name>_<pkg_version>\n\
  -q qmake    The name or full path of the qmake utility. In some\n\
              distributives, qmake for Qt 6 is named qmake6.\n\
  -s suffux   The suffix that will be added to the package name after the\n\
              version and before the architecture type.\n\
              Nothing is added by default.\n\
  -v version  Define the package version. By default version gettings from\n\
              the version.h file.\n\
\n\
It is assumed that the necessary dependencies and tools are already installed\n\
in the system.\n\
\n\
The created package is saved in the current working directory.\
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
# PACKAGE_INSTALL_PREFIX
# PACKAGE_BINS
# PACKAGE_DEBIAN_DESCRIPTION
source ${SCRIPT_DIR}/package.cfg

function echo_step()
{
    [ -v MODE_QUIET ] && return
    echo -e "\n================================================================================"
    echo -e "$1"
    echo -e "================================================================================\n"
}

# Returns 0 if there is no 'key' value in the 'list' array.
# param:
#   key    - key
#   list[] - list of a keys
function is_uniquue () {
    local item

    for item in "${@:2}"; do
        [[ ${item} == "${1}" ]] && return 1
    done

    return 0
}

# Returns a list of libraries on which the binaries depend
# param:
#   bins[] - list of a binaries
function get_libs () {
    local app
    local lib
    local lib_list

    for app in "${@}"; do
        for lib in $(objdump -p "${app}" | grep NEEDED | awk '{print $2}'); do
            if is_uniquue "${lib}" "${lib_list[@]}"; then
                lib_list+=("${lib}")
            fi
        done
    done

    echo "${lib_list[@]}"
}

# Returns a list of packages on which the binaries depend
# param:
#   arch   - architecture name
#   bins[] - list of a binaries
function get_deb_packages () {
    local pack
    local pack_list

    for pack in $(dpkg -S $(get_libs "${@:2}") | grep "${1}" | awk -F: '{print $1}'); do
        if is_uniquue "${pack}" "${pack_list[@]}"; then
            pack_list+=("${pack}")
        fi
    done

    echo "${pack_list[@]}"
}

# Returns a list of packages on which the binaries depend
# param:
#   arch   - architecture name
#   bins[] - list of a binaries
function get_deb_dependies () {
    local pack
    local dep_list
    local comma=0

    for pack in $(get_deb_packages "${1}" "${@:2}"); do
        ver=$(dpkg-query -f='${Version}' -W "${pack}:${1}")
        [[ ${comma} -eq 1 ]] && dep_list+=", "
        dep_list+="${pack} (>= ${ver})"
        comma=1
    done

    echo "${dep_list}"
}

QMAKE=qmake
BUILD_DIR=${SOURCE_DIR}/build
PACKAGE_VERSION=$(get_version)
PACKAGE_ARCH=$(dpkg --print-architecture)

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
PACKAGE_FULL_NAME="${PACKAGE_NAME}-${PACKAGE_VERSION}${PACKAGE_SUFFIX}_${PACKAGE_ARCH}"

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
if [ -v FORCE_VERSION ]; then
    QMAKE_OPTIONS="VERSION=$PACKAGE_VERSION "
fi
QMAKE_OPTIONS+="PREFIX=$PACKAGE_INSTALL_PREFIX $SOURCE_DIR"
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
mkdir -p "$PACKAGE_DIR"/DEBIAN
make install INSTALL_ROOT="$PACKAGE_DIR"
cd "$OLD_PWD"

echo_step "Create package"
echo -n "Getting depends..."
for bin_obj in "${PACKAGE_BINS[@]}"; do
    BINS+=("$PACKAGE_DIR/$PACKAGE_INSTALL_PREFIX/${bin_obj}")
done
PACKAGE_DEPENDS=$(get_deb_dependies "$PACKAGE_ARCH" ${BINS[@]})
echo " Done"

cat <<EOF > "$PACKAGE_DIR"/DEBIAN/control
Package: $PACKAGE_NAME
Version: $PACKAGE_VERSION
Section: devel
Priority: optional
Depends: $PACKAGE_DEPENDS
Architecture: $PACKAGE_ARCH
Maintainer: Nick Egorrov <nicegorov@yandex.ru>
Description: $PACKAGE_DEBIAN_DESCRIPTION
Installed-Size: $(du -s "$PACKAGE_DIR" | tr -cd 0-9)
EOF

if [ -f "${PACKAGE_FULL_NAME}.deb" ]; then
    echo "Remove old portable ${PACKAGE_FULL_NAME}.deb"
    rm "${PACKAGE_FULL_NAME}.deb"
fi
dpkg-deb -v --build "$PACKAGE_DIR" "${PACKAGE_FULL_NAME}.deb"
