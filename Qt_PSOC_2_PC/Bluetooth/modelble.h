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

// UUIDs
#define UUID_DEVICE     "{0000E750-0000-1000-8000-00805F9B34FB}"
#define UUID_SENSORS    0xE751
#define UUID_CONTROL    0xE752
#define UUID_STATUS     0xE753

// Byte masks for the status characteristic.
#define STATUS_READY_BYTE_MASK          0x00
#define STATUS_ACQUIRING_BYTE_MASK      0x01
#define STATUS_NO_MORE_SPACE_BYTE_MASK  0x02
#define STATUS_DATA_ACQUIRED_BYTE_MASK  0x03
#define STATUS_SENDING_BYTE_MASK        0x04
#define STATUS_NO_MORE_DATA_BYTE_MASK   0x05


class ModelBLE : public QStandardItemModel
{
    Q_OBJECT

public:
    static ModelBLE *getInstance();
    static void deleteInstance();
    ~ModelBLE();

    Device *PSOC = nullptr;
    GUID PSOCguid;

    Characteristic *Char_Sensors = nullptr;
    Characteristic *Char_Control = nullptr;
    Characteristic *Char_Status = nullptr;

    CapSense *Capsense = nullptr;

    static void newDataReceived(__in BTH_LE_GATT_EVENT_TYPE EventType, __in PVOID EventOutParameter, __in PVOID Context);
    static void newStatusReceived(__in BTH_LE_GATT_EVENT_TYPE EventType, __in PVOID EventOutParameter, __in PVOID Context);

    Status StatusFlags;

signals:
    void newCapSenseValuesReceived(sensors newValues);
    void statusUpdate(Status flags);

private:
    ModelBLE();
    static ModelBLE *instance;
};


void ErrorDescription(HRESULT hr);
