#-------------------------------------------------
#
# Project created by QtCreator 2015-12-13T22:52:36
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets serialport printsupport svg

TARGET = sensorreader
TEMPLATE = app

TRANSLATIONS = sr_ja.ts

CONFIG += C++11

SOURCES += main.cpp\
        widget.cpp \
    wavegraphwidget.cpp \
    serialsensormanager.cpp \
    graph.cpp \
    sensorparameters.cpp \
    abstractdatafilter.cpp \
    magdatafilter.cpp \
    gyrodatafilter.cpp \
    accdatafilter.cpp \
    tempdatafilter.cpp \
    pressdatafilter.cpp \
    connectdialog.cpp \
    writefileworker.cpp \
    logicaldrivedialog.cpp \
    progressdialog.cpp \
    micomfs_dev.c \
    micomfs.c \
    fsfileselectdialog.cpp \
    csvwritefileworker.cpp \
    tabledatareader.cpp \
    dataselectdialog.cpp \
    analyzefiledialog.cpp \
    analyzeworker.cpp \
    openfiledialog.cpp \
    gpsdatafilter.cpp

HEADERS  += widget.h \
    wavegraphwidget.h \
    serialsensormanager.h \
    graph.h \
    sensorparameters.h \
    device_id.h \
    abstractdatafilter.h \
    tempdatafilter.h \
    pressdatafilter.h \
    magdatafilter.h \
    gyrodatafilter.h \
    accdatafilter.h \
    connectdialog.h \
    writefileworker.h \
    micomfs.h \
    logicaldrivedialog.h \
    progressdialog.h \
    micomfs_dev.h \
    fsfileselectdialog.h \
    csvwritefileworker.h \
    tabledatareader.h \
    dataselectdialog.h \
    analyzefiledialog.h \
    analyzeworker.h \
    openfiledialog.h \
    gpsdatafilter.h

FORMS    += widget.ui \
    connectdialog.ui \
    progressdialog.ui \
    logicaldrivedialog.ui \
    fsfileselectdialog.ui \
    dataselectdialog.ui \
    analyzefiledialog.ui \
    openfiledialog.ui

win32 {
    RC_FILE = win.rc
}

DISTFILES += \
    win.rc \
    sensorreader.exe.manifest
