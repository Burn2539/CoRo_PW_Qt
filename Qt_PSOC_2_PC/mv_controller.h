#pragma once

/*****************************************************************************
* Included headers
*****************************************************************************/
#include "main.h"
#include "Bluetooth/modelble.h"

class MainWindow;

class MV_Controller : QObject
{
    Q_OBJECT

public:
    MV_Controller(MainWindow *parent);
    ~MV_Controller();

    //Services
    int getNumServices();

    //Characteristics
    int getNumCharacteristics(int serviceIndex);
    bool isCharacteristicNameEmpty(int serviceIndex, int characteristicIndex);
    QString getCharacteristicName(int serviceIndex, int characteristicIndex);
    Characteristic *getCharacteristicAddress(int serviceIndex, int characteristicIndex);

    //Current characteristic
    void setCurrChar(Characteristic *address);
    bool isCurrCharEmpty();
    bool isCurrCharDataModelEmpty();
    CapSense *getCurrCharDataModelAddress();
    int getNumSensors();
    void validateAndFillValues();
    void buildCurrCharDataModel();
    int getNumRowsCurrCharDataModel();
    quint16 getCurrCharValue(int row, int sensor);
    double getCurrCharCoM(int row);
    void generateKeys(double totalTime);
    double getCurrCharKey(int index);
    void clearCurrCharDataModel();
    bool isSynchronous();

    //CapSense
    void subscribeToSensorsNotifications(bool notifications, bool indications);
    void unsubscribeToSensorsNotifications();
    void startAcquisition(bool sendDataSynchronously);
    void stopAcquisition(bool sendDataSynchronously);
    void startSendingData();
    void stopSendingData();
    void startTimer();
    double getElapsedTime();
    QTime *getTimerAddress();

    //Status
    void subscribeToStatusNotifications(bool notifications, bool indications);
    void unsubscribeToStatusNotifications();
    void readStatusFlags();
    bool isStillAcquiring();
    bool isReady();


private:
    ModelBLE *model;
    MainWindow *view;
    Characteristic *CurrChar = nullptr;


signals:
    void newValuesReceivedFromModel_signal(sensors newValues);
    void statusUpdateFromModel_signal(Status flags);


private slots:
    void newValuesReceivedFromModel_slot(sensors newValues);
    void statusUpdateFromModel_slot(Status flags);

};
