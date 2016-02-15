#-------------------------------------------------
#
# Project created by QtCreator 2013-10-17T18:46:18
#
#-------------------------------------------------

QT      += core gui
QT      += printsupport
QT      += network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Disc
TEMPLATE = app

CONFIG   += c++11

SOURCES += main.cpp\
        mainwindow.cpp \
    element.cpp \
    calculator.cpp \
    qcustomplot.cpp \
    qtplotter.cpp \
    chargestatelistwindow.cpp \
    dipolescanner.cpp \
    qcodesysnv.cpp \
    popup.cpp

HEADERS  += mainwindow.h \
    element.h \
    calculator.h \
    qcustomplot.h \
    qtplotter.h \
    chargestatelistwindow.h \
    dipolescanner.h \
    qcodesysnv.h \
    popup.h

FORMS    += mainwindow.ui \
    chargestatelistwindow.ui \
    popup.ui

RESOURCES +=

OTHER_FILES += \
    LICENSE \
    README.md
