SOURCES += edytornc.cpp \
    main.cpp \
    highlighter.cpp \
    mdichild.cpp \
    customfiledialog.cpp \
    dialogs.cpp \
    findinf.cpp
HEADERS += edytornc.h \
    highlighter.h \
    mdichild.h \
    commoninc.h \
    customfiledialog.h \
    dialogs.h \
    findinf.h
TEMPLATE = app
CONFIG += warn_on \
    thread \
    qt \
    debug
TARGET = ../bin/edytornc
RESOURCES = application.qrc
RC_FILE = edytornc.rc
CONFIG -= release
FORMS += i2mdialog.ui \
    feedsdialog.ui \
    renumberdialog.ui \
    dotdialog.ui \
    triangledialog.ui \
    bhctabform.ui \
    bhcdialog.ui \
    mdichildform.ui
