#-------------------------------------------------
#
# Project created by QtCreator 2013-09-20T17:29:50
#
#-------------------------------------------------

QT       += core gui phonon

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = OnlineRadio
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui \
    about.ui

RESOURCES += \
    img.qrc

OTHER_FILES += \
    ../README.md
