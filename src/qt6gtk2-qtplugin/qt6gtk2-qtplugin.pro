include(../../qt6gtk2.pri)

TARGET = qt6gtk2

QT += core-private gui-private

HEADERS += \
    qt6gtk2theme.h \
    qt6gtk2dialoghelpers.h

SOURCES += \
        main.cpp \
    qt6gtk2theme.cpp \
    qt6gtk2dialoghelpers.cpp

TEMPLATE = lib
TARGET = qt6gtk2
CONFIG += plugin \
          link_pkgconfig \

PKGCONFIG += gtk+-2.0 x11

target.path = $$PLUGINDIR/platformthemes
INSTALLS += target
