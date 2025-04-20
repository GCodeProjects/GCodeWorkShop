#!/bin/sh

PROJECT_ROOT_DIR=$(realpath $(dirname $0)/../)
APP_BIN_DIR=$PROJECT_ROOT_DIR/bin/release
DLL_DIR=""
DEFAULT_INSTALL_DIR="C:/GCodeWorkShop"
INSTALL_DIR=""

set -e

usage() {
  echo  -e "\
Usage: `basename $0` [-d dll_dir] [-i [dir]]\n\
Deploy and install application.\n\
\n\
    -d dll_dir Directory where places DLL. By default uses directory\n\
               where qmake places.\n\
    -i [dir]   Install application in dir (if defined) or in\n\
               $DEFAULT_INSTALL_DIR\n\
" 1>&2
}

exit_abnormal() {
  usage
  exit 1
}

while getopts ":i:d:h" options; do
  case "${options}" in
    d)
      DLL_DIR=${OPTARG}
      ;;
    i)
      INSTALL_DIR=${OPTARG}
      ;;
    h)
      usage
      exit 0
      ;;
    :)
      if [ ${OPTARG} = "i" ]; then
        INSTALL_DIR=${DEFAULT_INSTALL_DIR}
      else
        echo "Error: option '-${OPTARG}' requires an argument.\n"
        exit_abnormal
      fi
      ;;
    *)
      echo "Error: unknown option '-${OPTARG}'.\n"
      exit_abnormal
      ;;
  esac
done

if [ "$DLL_DIR" = "" ]; then
  DLL_DIR=$(dirname $(which qmake))
fi

DLL_LIST="\
    libbrotlicommon.dll\
    libbrotlidec.dll\
    libbz2-1.dll\
    libdouble-conversion.dll\
    libfreetype-6.dll\
    libglib-2.0-0.dll\
    libgraphite2.dll\
    libharfbuzz-0.dll\
    libiconv-2.dll\
    libicudt72.dll\
    libicuin72.dll\
    libicuio72.dll\
    libicutest72.dll\
    libicutu72.dll\
    libicuuc72.dll\
    libintl-8.dll\
    libmd4c.dll\
    libpcre2-16-0.dll\
    libpcre2-8-0.dll\
    libpng16-16.dll\
    libzstd.dll\
    zlib1.dll"

echo ""
echo "Copying Qt libraries..."
windeployqt --compiler-runtime $APP_BIN_DIR
echo "OK"

echo ""
echo "Copying additional libraries..."
for DLL in $DLL_LIST
do
  echo "  $DLL"
  cp $DLL_DIR/$DLL $APP_BIN_DIR
done
echo "OK"

if [ "$INSTALL_DIR" = "" ]; then
  exit
fi

echo ""
echo "Application installation."
echo "Destination folder: $INSTALL_DIR"
mkdir -p $INSTALL_DIR/bin $INSTALL_DIR/doc $INSTALL_DIR/examples $INSTALL_DIR/lang
cp -r $PROJECT_ROOT_DIR/COPYING                                     $INSTALL_DIR
cp -r $PROJECT_ROOT_DIR/COPYING.LESSER                              $INSTALL_DIR
cp -r $APP_BIN_DIR/*                                                $INSTALL_DIR/bin
cp -r $PROJECT_ROOT_DIR/README.md                                   $INSTALL_DIR/doc
cp -r $PROJECT_ROOT_DIR/doc/SerialTransmission_Help.html            $INSTALL_DIR/doc
cp -r $PROJECT_ROOT_DIR/examples/*                                  $INSTALL_DIR/examples
cp -r $PROJECT_ROOT_DIR/lang/*.qm                                   $INSTALL_DIR/lang
echo "OK"
