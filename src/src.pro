IL######################################################################
# Automatically generated by qmake (3.0) ?? 11? 30 09:55:49 2017
######################################################################

QT += core gui widgets svg dbus

TEMPLATE = app
TARGET = deepin-font-installer
INCLUDEPATH += .
CONFIG += c++11 link_pkgconfig
PKGCONFIG += dtkwidget freetype2 fontconfig glib-2.0

#SUBDIRS += lib
#DEPENDPATH += $$PWD/../lib
#LIBS += -L$$OUT_PWD/../lib -llibdeepin-font-installer

# Input
HEADERS += mainwindow.h homepage.h singlefilepage.h multifilepage.h listview.h utils.h listitem.h fontdata.h
SOURCES += main.cpp mainwindow.cpp homepage.cpp singlefilepage.cpp multifilepage.cpp listview.cpp utils.cpp listitem.cpp fontdata.cpp

HEADERS += ../lib/dfontinfo.h
SOURCES += ../lib/dfontinfo.cpp

RESOURCES += ../deepin-font-installer.qrc
