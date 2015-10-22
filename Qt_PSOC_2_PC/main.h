#pragma warning(disable:4068)
#pragma once

/* General librairies. */
#include <QThread>
#include <QApplication>
#include <QtGlobal>
#include <QMainWindow>
#include <QStringList>
#include <QDebug>
#include <QVector>
#include <QStandardItemModel>
#include <QElapsedTimer>
#include "qcustomplot.h"

#define NUM_SENSORS 5

struct sensors {
    quint16 sensor[NUM_SENSORS];
};
Q_DECLARE_METATYPE(sensors)
