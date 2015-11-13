#pragma warning(disable:4068)
#pragma once

/* General librairies. */
#include <QThread>
#include <QApplication>
#include <QtGlobal>
#include <QMainWindow>
#include <QDialog>
#include <QStringList>
#include <QDebug>
#include <QVector>
#include <QStandardItemModel>
#include <QElapsedTimer>
#include "qcustomplot.h"

#define NUM_SENSORS 10


struct BLEsensor {
    quint32 Data;
    quint16 Value;
    quint16 CRC;
};


struct sensors {
    quint16 sensor[NUM_SENSORS];
};
Q_DECLARE_METATYPE(sensors)


struct Status {
    bool Ready;
    bool Acquiring;
    bool NoMoreSpace;
    bool DataAcquired;
    bool Sending;
    bool NoMoreData;
};
Q_DECLARE_METATYPE(Status)
