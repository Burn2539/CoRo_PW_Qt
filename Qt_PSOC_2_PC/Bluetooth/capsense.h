#pragma once

/*****************************************************************************
* Included headers
*****************************************************************************/
#include "modelBLE.h"

class CapSense : public QAbstractTableModel
{
public:
    CapSense();
    ~CapSense();

    QTime *timer;

    QVector<quint8> rawData;
    QVector<double> keys;
    QVector<sensors> values;
    QVector<double> centerMass;

    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE ;
    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    bool insertRows(int position, int rows, const QModelIndex &index = QModelIndex()) Q_DECL_OVERRIDE;
    bool removeRows(int position, int rows, const QModelIndex &index = QModelIndex()) Q_DECL_OVERRIDE;
};


