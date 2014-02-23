#-------------------------------------------------
#
# Project created by QtCreator 2013-06-11T16:42:32
#
#-------------------------------------------------

QT       += core gui

LIBS += $(SUBLIBS) -lQt53D -lQt5OpenGL -lQt5Widgets -lQt5Gui -lQt5Core -lQt5Multimedia -lQt5MultimediaWidgets

greaterThan(QT_MAJOR_VERSION, 4):
QT += widgets
QT += sql

TARGET = EMERGE_Capstone
TEMPLATE = app

SOURCES += \
    main.cpp \
    src/uimodel.cpp \
    src/uimainwindow.cpp \
    src/uigraphview.cpp \
    src/uicameraview.cpp


HEADERS  += \
    inc/uimodel.h\
    inc/uimainwindow.h \
    inc/uigraphview.h \
    inc/uicameraview.h \
    inc/uiutil.h \
    inc/uirobot.h
