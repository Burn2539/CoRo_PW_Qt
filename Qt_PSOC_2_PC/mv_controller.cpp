#include "mv_controller.h"
#include "mainwindow.h"
#include "Bluetooth/modelble.h"

MV_Controller::MV_Controller(MainWindow *parent)
{
    ModelBLE::deleteInstance();
    this->model = ModelBLE::getInstance();

    crcInit();

    // Transfer the 'newCapSenseValues' signal to the view
    connect(this->model, SIGNAL(newCapSenseValuesReceived(sensors)), this, SLOT(newValuesReceivedFromModel_slot(sensors)));
    connect(this, SIGNAL(newValuesReceivedFromModel_signal(sensors)), parent, SLOT(newValuesReceived_updateView(sensors)));

    // Transfer the 'statusUpdate' signal to the view
    connect(this->model, SIGNAL(statusUpdate(Status)), this, SLOT(statusUpdateFromModel_slot(Status)));
    connect(this, SIGNAL(statusUpdateFromModel_signal(Status)), parent, SLOT(statusUpdate_updateView(Status)));
}


MV_Controller::~MV_Controller()
{
    delete model;
}


int MV_Controller::getNumServices()
{
    return model->PSOC->numServices;
}


int MV_Controller::getNumCharacteristics(int serviceIndex)
{
    return model->PSOC->services[serviceIndex]->numCharacteristics;
}


bool MV_Controller::isCharacteristicNameEmpty(int serviceIndex, int characteristicIndex)
{
    return model->PSOC->services[serviceIndex]->characteristics[characteristicIndex]->name.isEmpty();
}


QString MV_Controller::getCharacteristicName(int serviceIndex, int characteristicIndex)
{
    return model->PSOC->services[serviceIndex]->characteristics[characteristicIndex]->name;
}


Characteristic *MV_Controller::getCharacteristicAddress(int serviceIndex, int characteristicIndex)
{
    return model->PSOC->services[serviceIndex]->characteristics[characteristicIndex];
}


void MV_Controller::setCurrChar(Characteristic *address)
{
    CurrChar = address;
}


bool MV_Controller::isCurrCharEmpty()
{
    return CurrChar == nullptr;
}


bool MV_Controller::isCurrCharDataModelEmpty()
{
    return model->Capsense->values.isEmpty();
}


CapSense *MV_Controller::getCurrCharDataModelAddress()
{
    return model->Capsense;
}


int MV_Controller::getNumSensors()
{
    return NUM_SENSORS;
}


void MV_Controller::subscribeToSensorsNotifications(bool notifications, bool indications)
{
    model->Char_Sensors->subscribeToNotification(notifications, indications, ModelBLE::newDataReceived);
}


void MV_Controller::unsubscribeToSensorsNotifications()
{
    model->Char_Sensors->unsubscribeToNotification();
}


void MV_Controller::subscribeToStatusNotifications(bool notifications, bool indications)
{
    model->Char_Status->subscribeToNotification(notifications, indications, ModelBLE::newStatusReceived);
}


void MV_Controller::unsubscribeToStatusNotifications()
{
    model->Char_Status->unsubscribeToNotification();
}


void MV_Controller::buildCurrCharDataModel()
{
    model->Capsense->insertRows(0, model->Capsense->values.size());
}


int MV_Controller::getNumRowsCurrCharDataModel()
{
    return model->Capsense->rowCount();
}


quint16 MV_Controller::getCurrCharValue(int row, int sensor)
{
    return model->Capsense->values[row].sensor[sensor];
}


void MV_Controller::clearCurrCharDataModel()
{
    model->Capsense->removeRows(0, getNumRowsCurrCharDataModel());
    model->Capsense->values.clear();
    model->Capsense->keys.clear();
}


void MV_Controller::newValuesReceivedFromModel_slot(sensors newValues)
{
    emit(this->newValuesReceivedFromModel_signal(newValues));
}


void MV_Controller::statusUpdateFromModel_slot(Status flags)
{
    emit(this->statusUpdateFromModel_signal(flags));
}


void MV_Controller::startTimer()
{
    model->Capsense->timer->start();
}


double MV_Controller::getElapsedTime()
{
    return model->Capsense->timer->elapsed();
}


void MV_Controller::generateKeys(double totalTime)
{
    for (double i = 1; i <= model->Capsense->rowCount(); i++)
        model->Capsense->keys.push_back( i * (totalTime / model->Capsense->rowCount()) );
}


double MV_Controller::getCurrCharKey(int index)
{
    return model->Capsense->keys[index];
}


QTime *MV_Controller::getTimerAddress()
{
    return model->Capsense->timer;
}


void MV_Controller::startAcquisition(bool sendDataSynchronously)
{
    PBTH_LE_GATT_CHARACTERISTIC_VALUE value = new BTH_LE_GATT_CHARACTERISTIC_VALUE;
    quint8 acquireData = true;
    quint8 sendData = false;

    value->Data[0] = acquireData;
    value->Data[1] = sendData;
    value->Data[2] = sendDataSynchronously;
    value->DataSize = 3;

    model->Char_Control->writeValue(value);
}


void MV_Controller::stopAcquisition(bool sendDataSynchronously)
{
    PBTH_LE_GATT_CHARACTERISTIC_VALUE value = new BTH_LE_GATT_CHARACTERISTIC_VALUE;
    quint8 acquireData = false;
    quint8 sendData = false;

    value->Data[0] = acquireData;
    value->Data[1] = sendData;
    value->Data[2] = sendDataSynchronously;
    value->DataSize = 3;

    model->Char_Control->writeValue(value);
}


void MV_Controller::startSendingData()
{
    PBTH_LE_GATT_CHARACTERISTIC_VALUE value = new BTH_LE_GATT_CHARACTERISTIC_VALUE;
    quint8 acquireData = false;
    quint8 sendData = true;
    quint8 sendDataSynchronously = false;

    value->Data[0] = acquireData;
    value->Data[1] = sendData;
    value->Data[2] = sendDataSynchronously;
    value->DataSize = 3;

    model->Char_Control->writeValue(value);
}


void MV_Controller::stopSendingData()
{
    PBTH_LE_GATT_CHARACTERISTIC_VALUE value = new BTH_LE_GATT_CHARACTERISTIC_VALUE;
    quint8 acquireData = false;
    quint8 sendData = false;
    quint8 sendDataSynchronously = false;

    value->Data[0] = acquireData;
    value->Data[1] = sendData;
    value->Data[2] = sendDataSynchronously;
    value->DataSize = 3;

    model->Char_Control->writeValue(value);
}


void MV_Controller::readStatusFlags()
{
    PBTH_LE_GATT_CHARACTERISTIC_VALUE value;
    Status flags;

    model->Char_Status->readValue(&value);

    flags.Ready = value->Data[STATUS_READY_BYTE_MASK];
    flags.Acquiring = value->Data[STATUS_ACQUIRING_BYTE_MASK];
    flags.NoMoreSpace = value->Data[STATUS_NO_MORE_SPACE_BYTE_MASK];
    flags.DataAcquired = value->Data[STATUS_DATA_ACQUIRED_BYTE_MASK];
    flags.Sending = value->Data[STATUS_SENDING_BYTE_MASK];
    flags.NoMoreData = value->Data[STATUS_NO_MORE_DATA_BYTE_MASK];

    emit(statusUpdateFromModel_signal(flags));
}


bool MV_Controller::isStillAcquiring()
{
    return model->StatusFlags.Acquiring;
}


bool MV_Controller::isReady()
{
    return model->StatusFlags.Ready;
}
