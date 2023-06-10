
APP_ARCH = $$QMAKE_HOST.arch

CXX_RUNTIME = $$system(where $$QMAKE_CXX)
CXX_RUNTIME = $$first(CXX_RUNTIME)
CXX_RUNTIME = $$dirname(CXX_RUNTIME)

QMAKE_EXTRA_TARGETS += nsis

mingw:NSIS_EXTRA_FILES = "$${PWD}/nsis/mingw.nsi"

nsis.commands = makensis \
    -DAPP_ARCH=$$APP_ARCH \
    -DINSTALLER_FILE="$${INSTALLER_DIR}/GCodeWorkShop_$${APP_ARCH}_Setup.exe" \
    -DPROJECT_ROOT="$$PROJECT_ROOT_PATH" \
    -DAPP_BINS="$$APP_BINS" \
    -DCXX_RUNTIME="$$CXX_RUNTIME" \
    -DQT_HOST_BINS="$$[QT_HOST_BINS]" \
    -DQT_HOST_DATA="$$[QT_HOST_DATA]" \
    "$${PWD}/nsis/app.nsi" \
    "$${NSIS_EXTRA_FILES}"
