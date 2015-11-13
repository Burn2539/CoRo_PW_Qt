#-------------------------------------------------
#
# Project created by QtCreator 2015-10-22T10:38:20
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = Qt_PSOC_2_PC
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    mv_controller.cpp \
    qcustomplot.cpp \
    Bluetooth/capsense.cpp \
    Bluetooth/Characteristic.cpp \
    Bluetooth/crc.cpp \
    Bluetooth/Descriptor.cpp \
    Bluetooth/Device.cpp \
    Bluetooth/modelble.cpp \
    Bluetooth/Service.cpp \
    settingswindow.cpp \
    gattprofilewindow.cpp \
    Bluetooth/l2cap.cpp

HEADERS  += mainwindow.h \
    mv_controller.h \
    qcustomplot.h \
    Bluetooth/capsense.h \
    Bluetooth/Characteristic.h \
    Bluetooth/crc.h \
    Bluetooth/Descriptor.h \
    Bluetooth/Device.h \
    Bluetooth/modelble.h \
    Bluetooth/Service.h \
    main.h \
    settingswindow.h \
    gattprofilewindow.h \
    Bluetooth/l2cap.h

FORMS    += mainwindow.ui \
    settingswindow.ui \
    gattprofilewindow.ui
