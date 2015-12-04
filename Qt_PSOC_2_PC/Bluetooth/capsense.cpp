/*****************************************************************************
* Included headers
*****************************************************************************/
#include "capsense.h"

/*************************************************************************
*
*	CONSTRUCTOR
*
**************************************************************************/
CapSense::CapSense()
{
    timer = new QTime;
    rawData.reserve(10000);
    values.reserve(5000);
    keys.reserve(5000);
    centerMass.reserve(5000);
}


/*************************************************************************
*
*	DESTRUCTOR
*
**************************************************************************/
CapSense::~CapSense()
{
    rawData.clear();
    values.clear();
    keys.clear();
    centerMass.clear();
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
    /**********************************************************
     * Sensor1 * Sensor2 * Sensor3 *   ...   * Center of mass *
     **********************************************************/
    return NUM_SENSORS + 1;
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
        if (index.column() < NUM_SENSORS)
            return this->values.at(index.row()).sensor[index.column()];
        else
            return this->centerMass.at(index.row());
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
            if (section < NUM_SENSORS)
                return QString("Sensor %1").arg(section + 1);
            else
                return QString("Center of mass");
        }
        else if (orientation == Qt::Vertical)
            //return QString::number(keys[section], 'f', 3);
            return QString("%1").arg(section + 1);
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

    beginResetModel();
    endResetModel();

    return TRUE;
}
