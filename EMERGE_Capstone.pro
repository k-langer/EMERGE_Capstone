#-------------------------------------------------
#
# Project created by QtCreator 2013-06-11T16:42:32
#
#-------------------------------------------------

QT       += core gui

LIBS += $(SUBLIBS) -lQt53D -lQt5OpenGL -lQt5Widgets -lQt5Gui -lQt5Core

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = EMERGE_Capstone
TEMPLATE = app

SOURCES += main.cpp\
    src/uimainwindow.cpp \
    src/uigraphview.cpp

HEADERS  += mainwindow.h \
    inc/uimainwindow.h \
    inc/uigraphview.h \
    inc/util.h