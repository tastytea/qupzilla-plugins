include(../../defines.pri)
CONF += debug

TARGET = $$qtLibraryTarget(PassPasswords)

SOURCES += passplugin.cpp \
    passpasswordbackend.cpp \
    passpasswords_settings.cpp

HEADERS += passplugin.h \
    passpasswordbackend.h \
    passpasswords_settings.h

RESOURCES += passpasswords.qrc

TRANSLATIONS += \
    translations/de_DE.ts \

FORMS += passpasswords_settings.ui


PLUGIN_DIR = $$PWD
include(../../plugins.pri)
