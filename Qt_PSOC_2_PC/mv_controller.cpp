#include "mv_controller.h"
#include "mainwindow.h"
#include "Bluetooth/modelble.h"

MV_Controller::MV_Controller(MainWindow *parent)
{
    this->model = ModelBLE::getInstance();
    crcInit();

    connect(this->model, SIGNAL(newCapSenseValuesReceived(sensors)), this, SLOT(newValuesReceivedFromModel_slot(sensors)));
    connect(this, SIGNAL(newValuesReceivedFromModel_signal(sensors)), parent, SLOT(newValuesReceived_updateView(sensors)));
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
    return CurrChar->Capsense->values.isEmpty();
}


CapSense *MV_Controller::getCurrCharDataModelAddress()
{
    return CurrChar->Capsense;
}


int MV_Controller::getNumSensors()
{
    return NUM_SENSORS;
}


void MV_Controller::subscribeToCurrCharNotifications(bool notifications, bool indications)
{
    CurrChar->subscribeToNotification(notifications, indications, ModelBLE::newSensorDataReceived);
}


void MV_Controller::unsubscribeToCurrCharNotifications()
{
    CurrChar->unsubscribeToNotification();
}


void MV_Controller::buildCurrCharDataModel()
{
    CurrChar->Capsense->insertRows(0, CurrChar->Capsense->values.size());
}


int MV_Controller::getNumRowsCurrCharDataModel()
{
    return CurrChar->Capsense->rowCount();
}


quint16 MV_Controller::getCurrCharValue(int row, int sensor)
{
    return CurrChar->Capsense->values[row].sensor[sensor];
}


void MV_Controller::clearCurrCharDataModel()
{
    CurrChar->Capsense->removeRows(0, getNumRowsCurrCharDataModel());
    CurrChar->Capsense->values.clear();
    CurrChar->Capsense->keys.clear();
}


void MV_Controller::newValuesReceivedFromModel_slot(sensors newValues)
{
    emit(this->newValuesReceivedFromModel_signal(newValues));
}


void MV_Controller::startTimer()
{
    CurrChar->Capsense->timer->start();
}


double MV_Controller::getElapsedTime()
{
    return CurrChar->Capsense->timer->elapsed();
}


void MV_Controller::generateKeys(double totalTime)
{
    for (double i = 0; i < CurrChar->Capsense->rowCount(); i++)
        CurrChar->Capsense->keys.push_back( i * (totalTime / CurrChar->Capsense->rowCount()) );
}


double MV_Controller::getCurrCharKey(int index)
{
    return CurrChar->Capsense->keys[index];
}


QTime *MV_Controller::getTimerAddress()
{
    return CurrChar->Capsense->timer;
}
