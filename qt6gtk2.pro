TEMPLATE = subdirs

SUBDIRS += src/qt6gtk2-qtplugin src/qt6gtk2-style

include(qt6gtk2.pri)

message (PLUGINDIR=$$PLUGINDIR)
