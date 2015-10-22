#pragma once

#include "main.h"

/* Librairies needed to use BLE on Windows. */
#include <SDKDDKVer.h>
#include <windows.h>
#include <setupapi.h>
#include <devguid.h>
#include <regstr.h>
#include <bthdef.h>
#include <Bluetoothleapis.h>
#pragma comment(lib, "SetupAPI")
#pragma comment(lib, "BluetoothApis.lib")
#pragma comment(lib, "Ole32.lib")

/* My librairies. */
#include "Bluetooth/crc.h"
#include "Bluetooth/Descriptor.h"
#include "Bluetooth/capsense.h"
#include "Bluetooth/Characteristic.h"
#include "Bluetooth/Service.h"
#include "Bluetooth/Device.h"

#define BYTE 8


class ModelBLE : public QStandardItemModel
{
    Q_OBJECT

public:
    static ModelBLE *getInstance();
    ~ModelBLE();

    Device *PSOC;
    GUID PSOCguid;

    static void newSensorDataReceived(__in BTH_LE_GATT_EVENT_TYPE EventType, __in PVOID EventOutParameter, __in PVOID Context);

signals:
    void newCapSenseValuesReceived(sensors newValues);

private:
    ModelBLE();
    static ModelBLE *instance;
};

void ErrorDescription(HRESULT hr);
