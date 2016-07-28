#-------------------------------------------------
#
# Project created by QtCreator 2015-04-26T10:15:49
#
#-------------------------------------------------

QT       += core gui
CONFIG   += C++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = Interpolation
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    qcustomplot.cpp \
    spinboxdelegate.cpp \
    interpolation.cpp \
    splinesinterpolation.cpp \
    aitkeninterpolation.cpp \
    getvalueatpointdialog.cpp

HEADERS  += mainwindow.h \
    qcustomplot.h \
    spinboxdelegate.h \
    interpolation.h \
    splinesinterpolation.h \
    aitkeninterpolation.h \
    getvalueatpointdialog.h

FORMS    += mainwindow.ui \
    getvalueatpointdialog.ui
