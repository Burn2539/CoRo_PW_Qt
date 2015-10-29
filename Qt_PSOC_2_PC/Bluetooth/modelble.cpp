#include "modelble.h"

struct BLEsensor {
    quint32 Data;
    quint16 Value;
    quint16 CRC;
};

ModelBLE *ModelBLE::instance = 0;

/*************************************************************************
*
*	SINGLETON
*
**************************************************************************/
ModelBLE *ModelBLE::getInstance()
{
    if (!instance)
        instance = new ModelBLE;
    return instance;
}


/*************************************************************************
*
*	CONSTRUCTOR
*
**************************************************************************/
ModelBLE::ModelBLE()
{
    instance = this;

    qRegisterMetaType<sensors>();
    qRegisterMetaType<Status>();

    /* Create the BLE device. */
    CLSIDFromString(TEXT(UUID_DEVICE), &instance->PSOCguid);
    instance->PSOC = new Device(instance->PSOCguid);

    /* Retrieve the whole data structure from the BLE device. */
    instance->PSOC->retrieveListServices();
    for (int i = 0; i < instance->PSOC->numServices; i++) {
        instance->PSOC->services[i]->retrieveListCharacteristics();
        for (int j = 0; j < instance->PSOC->services[i]->numCharacteristics; j++) {
            instance->PSOC->services[i]->characteristics[j]->retrieveListDescriptors();
            instance->PSOC->services[i]->characteristics[j]->getName();

            // Get the address of the Sensors, Control and Status characteristics
            if (instance->PSOC->services[i]->characteristics[j]->CharacteristicUuid.Value.ShortUuid == UUID_SENSORS)
                instance->Char_Sensors = instance->PSOC->services[i]->characteristics[j];
            else if (instance->PSOC->services[i]->characteristics[j]->CharacteristicUuid.Value.ShortUuid == UUID_CONTROL)
                instance->Char_Control = instance->PSOC->services[i]->characteristics[j];
            else if (instance->PSOC->services[i]->characteristics[j]->CharacteristicUuid.Value.ShortUuid == UUID_STATUS)
                instance->Char_Status = instance->PSOC->services[i]->characteristics[j];
        }
    }

    // Write in console if it could not find one the characteristic needed.
    if (instance->Char_Sensors == nullptr)
        qCritical() << "Could not find the Sensors characteristic per its UUID: " << UUID_SENSORS << endl;
    if (instance->Char_Control == nullptr)
        qCritical() << "Could not find the Control characteristic per its UUID: " << UUID_CONTROL << endl;
    if (instance->Char_Status == nullptr)
        qCritical() << "Could not find the Status characteristic per its UUID: " << UUID_STATUS << endl;

    // Reset the status flags.
    instance->StatusFlags.Ready = false;
    instance->StatusFlags.Acquiring = false;
    instance->StatusFlags.NoMoreSpace = false;
    instance->StatusFlags.DataAcquired = false;
    instance->StatusFlags.Sending = false;
    instance->StatusFlags.NoMoreData = false;

    /* Create the CapSense instance. */
    instance->Capsense = new CapSense;
}


/*************************************************************************
*
*	DESTRUCTOR
*
**************************************************************************/
ModelBLE::~ModelBLE()
{
    delete instance->PSOC;
    delete Capsense;
}


/*************************************************************************
*
* Function:		newSensorDataReceived()
*
* Description:	Receive the new sensor values sent by the BLE device.
*
* Notes:		Each sensor data is 4 bytes long.
*				The first 2 bytes represent the value of the sensor.
*				The last 2 bytes represent the CRC result of the value.
*
* Parameters:	Type of event, Data received and Additionnal information
*				you can send through the BluetoothGATTRegisterEvent
*				function call.
*
* Returns:		None defined.
*
**************************************************************************/
void ModelBLE::newDataReceived(__in BTH_LE_GATT_EVENT_TYPE EventType, __in PVOID EventOutParameter, __in PVOID Context)
{
    Q_UNUSED(EventType);

    PBLUETOOTH_GATT_VALUE_CHANGED_EVENT ValueChangedEventParameters = (PBLUETOOTH_GATT_VALUE_CHANGED_EVENT)EventOutParameter;
    Characteristic *Characteristic_context = (Characteristic *)Context;

    // Sensors values received.
    if (Characteristic_context->CharacteristicUuid.Value.ShortUuid == UUID_SENSORS)
    {
        BLEsensor sensor[NUM_SENSORS];
        sensors valuesToPush;
        const quint8 sensorDataSize = sizeof(quint32);

        /* For each sensors... */
        for (int i = 0; i < NUM_SENSORS; i++) {
            /* For each byte... */
            for (int j = 0; j < sensorDataSize; j++)
                /* Copy the byte into the array that holds the whole DWORD sent by the BLE device. */
                sensor[i].Data = sensor[i].Data << BYTE | unsigned(ValueChangedEventParameters->CharacteristicValue->Data[(sensorDataSize * i) + j]);

            /* Extract the sensor value and its CRC from the DWORD. */
            sensor[i].Value = sensor[i].Data >> (2 * BYTE) & 0xFFFF;
            sensor[i].CRC = sensor[i].Data & 0xFFFF;

            /* If the sensor value corresponds to the CRC, keep the value. Otherwise, replace it by NULL. */
            if (verifyCRC(sensor[i].Value, sensor[i].CRC))
                valuesToPush.sensor[i] = sensor[i].Value;
            else
                valuesToPush.sensor[i] = NULL;
        }

        /* Push all the sensor values received into the vector that holds all the sensor values. */
        instance->Capsense->values.push_back(valuesToPush);

        /* Signal to update the progress bars displaying the last values. */
        emit(instance->newCapSenseValuesReceived(valuesToPush));
    }
}


/*************************************************************************
*
* Function:		newStatusReceived()
*
* Description:	Receive the status values sent by the BLE device.
*
* Notes:		The flags are:
*                   Ready, Acquiring, NoMoreSpace, DataAcquired
*                   Sending, NoMoreData
*
* Parameters:	Type of event, Data received and Additionnal information
*				you can send through the BluetoothGATTRegisterEvent
*				function call.
*
* Returns:		None defined.
*
**************************************************************************/
void ModelBLE::newStatusReceived(__in BTH_LE_GATT_EVENT_TYPE EventType, __in PVOID EventOutParameter, __in PVOID Context)
{
    Q_UNUSED(EventType);

    PBLUETOOTH_GATT_VALUE_CHANGED_EVENT ValueChangedEventParameters = (PBLUETOOTH_GATT_VALUE_CHANGED_EVENT)EventOutParameter;
    Characteristic *Characteristic_context = (Characteristic *)Context;

    // Status update received.
    if (Characteristic_context->CharacteristicUuid.Value.ShortUuid == UUID_STATUS)
    {
        // Update every status flags.
        instance->StatusFlags.Ready = ValueChangedEventParameters->CharacteristicValue->Data[STATUS_READY_BYTE_MASK];
        instance->StatusFlags.Acquiring = ValueChangedEventParameters->CharacteristicValue->Data[STATUS_ACQUIRING_BYTE_MASK];
        instance->StatusFlags.NoMoreSpace = ValueChangedEventParameters->CharacteristicValue->Data[STATUS_NO_MORE_SPACE_BYTE_MASK];
        instance->StatusFlags.DataAcquired = ValueChangedEventParameters->CharacteristicValue->Data[STATUS_DATA_ACQUIRED_BYTE_MASK];
        instance->StatusFlags.Sending = ValueChangedEventParameters->CharacteristicValue->Data[STATUS_SENDING_BYTE_MASK];
        instance->StatusFlags.NoMoreData = ValueChangedEventParameters->CharacteristicValue->Data[STATUS_NO_MORE_DATA_BYTE_MASK];

        // Signal to update the status flags on the view
        emit(instance->statusUpdate(instance->StatusFlags));
    }
}


/*************************************************************************
*
* Function:		ErrorDescription()
*
* Description:	Print a message explaining the HRESULT error code.
*
* Notes:		https://msdn.microsoft.com/en-us/library/windows/desktop/ms687061(v=vs.85).aspx
*
* Parameters:	HRESULT error code.
*
* Returns:		None defined.
*
**************************************************************************/
void ErrorDescription(HRESULT hr)
{
    if (FACILITY_WINDOWS == HRESULT_FACILITY(hr))
        hr = HRESULT_CODE(hr);
    TCHAR* szErrMsg;

    if (FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&szErrMsg, 0, NULL) != 0)
    {
        qDebug("%s", szErrMsg);
        LocalFree(szErrMsg);
    }
    else
        qDebug("[Could not find a description for error # %#x.]\n", hr);
}
