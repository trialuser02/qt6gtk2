include(../../qt6gtk2.pri)

TEMPLATE = lib
TARGET = qt6gtk2-style
QT += core-private gui-private widgets-private

DEFINES += QT_NO_ANIMATION

# Input
HEADERS += qgtk2painter_p.h \
           qgtkglobal_p.h \
           qgtkpainter_p.h \
           qgtkstyle_p.h \
           qgtkstyle_p_p.h \
    qstylehelper_p.h
SOURCES += qgtk2painter.cpp qgtkpainter.cpp qgtkstyle.cpp qgtkstyle_p.cpp \
    plugin.cpp \
    qstylehelper.cpp

CONFIG += plugin \
          link_pkgconfig \

PKGCONFIG += gtk+-2.0 x11

target.path = $$PLUGINDIR/styles
INSTALLS += target
