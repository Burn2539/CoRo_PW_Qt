#include "capsense.h"

/*************************************************************************
*
*	CONSTRUCTOR
*
**************************************************************************/
CapSense::CapSense()
{
    timer = new QTime;
    values.reserve(5000);
}


/*************************************************************************
*
*	DESTRUCTOR
*
**************************************************************************/
CapSense::~CapSense()
{
    values.clear();
}


/*************************************************************************
*
*	NUMBER OF ROWS
*
**************************************************************************/
int CapSense::rowCount(const QModelIndex & /*parent*/) const
{
    return this->values.size();
}


/*************************************************************************
*
*	NUMBER OF COLUMNS
*
**************************************************************************/
int CapSense::columnCount(const QModelIndex & /*parent*/) const
{
    /***************************************************
     * Sensor1 * Sensor2 * Sensor3 * Sensor4 * Sensor5 *
     ***************************************************/
    return 5;
}


/*************************************************************************
*
*	DATA
*
**************************************************************************/
QVariant CapSense::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole)
    {
        return this->values.at(index.row()).sensor[index.column()];
    }
    return QVariant();
}


/*************************************************************************
*
*	HEADER DATA
*
**************************************************************************/
QVariant CapSense::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal) {
            switch (section)
            {
            case 0: return QString("Sensor 1");
            case 1: return QString("Sensor 2");
            case 2: return QString("Sensor 3");
            case 3: return QString("Sensor 4");
            case 4: return QString("Sensor 5");
            }
        }
        else if (orientation == Qt::Vertical) {
            return QString::number(keys[section], 'f', 4);
        }
    }
    return QVariant();
}


/*************************************************************************
*
*	INSERT ROWS
*
**************************************************************************/
bool CapSense::insertRows(int position, int rows, const QModelIndex &index)
{
    Q_UNUSED(index);

    beginInsertRows(QModelIndex(), position, position + rows - 1);
    endInsertRows();

    return TRUE;
}


/*************************************************************************
*
*	REMOVE ROWS
*
**************************************************************************/
bool CapSense::removeRows(int position, int rows, const QModelIndex &index)
{
    Q_UNUSED(index);

    beginRemoveRows(QModelIndex(), position, position + rows - 1);
    endRemoveRows();

    return TRUE;
}
