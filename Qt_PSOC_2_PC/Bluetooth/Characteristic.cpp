/*****************************************************************************
* Included headers
*****************************************************************************/
#include "Characteristic.h"

using namespace std;

/*************************************************************************
*
*	COSNTRUCTOR
*
**************************************************************************/
Characteristic::Characteristic(Service *_service, PBTH_LE_GATT_CHARACTERISTIC _characteristic)
{
	this->parentService = _service;

	this->AttributeHandle = _characteristic->AttributeHandle;
	this->CharacteristicUuid = _characteristic->CharacteristicUuid;
	this->CharacteristicValueHandle = _characteristic->CharacteristicValueHandle;
	this->HasExtendedProperties = _characteristic->HasExtendedProperties;
	this->IsBroadcastable = _characteristic->IsBroadcastable;
	this->IsIndicatable = _characteristic->IsIndicatable;
	this->IsNotifiable = _characteristic->IsNotifiable;
	this->IsReadable = _characteristic->IsReadable;
	this->IsSignedWritable = _characteristic->IsSignedWritable;
	this->IsWritable = _characteristic->IsWritable;
	this->IsWritableWithoutResponse = _characteristic->IsWritableWithoutResponse;
	this->ServiceHandle = _characteristic->ServiceHandle;
}


/*************************************************************************
*
*	DESCRUCTOR
*
**************************************************************************/
Characteristic::~Characteristic()
{
	if (this->notificationEnabled)
		this->unsubscribeToNotification();
	for (int i = 0; i < this->numDescriptors; i++)
		delete this->descriptors[i];

    qDebug() << "Characteristic has been deleted." << endl;
}


/*************************************************************************
*
* Function:		retrieveListDescriptors()
*
* Description:	Retrieve the list of all the descriptors available on
*				the BLE device.
*
* Notes:		Must have called getListCharacteristics before because
*				characteristicsBuffer is needed.
*
*				First send 0,NULL as the parameters to
*				BluetoothGATTGetDescriptors in order to get the
*				number of descriptors in descriptorBufferSize.
*
*				BluetoothGATTGetDescriptors:
*				https://msdn.microsoft.com/en-us/library/windows/hardware/hh450797(v=vs.85).aspx
*
* Parameters:	None.
*
* Returns:		Result of the second call of the
*				BluetoothGATTGetDescriptors.
*
**************************************************************************/
HRESULT Characteristic::retrieveListDescriptors()
{
	PBTH_LE_GATT_DESCRIPTOR descriptorsBuffer;
	USHORT descriptorBufferSize;

	/* Determine the size of the buffer required. */
	HRESULT hr = BluetoothGATTGetDescriptors(
		this->parentService->parentDevice->Handle,
		this,
		0,
		NULL,
		&descriptorBufferSize,
		BLUETOOTH_GATT_FLAG_NONE);

	if (HRESULT_FROM_WIN32(ERROR_MORE_DATA) != hr) {
        qWarning() << "\tNo descriptor found for this characteristic.";
        this->numDescriptors = 0;
		return hr;
	}

	/* Allocate space for the buffer. */
	if (descriptorBufferSize > 0) {
		descriptorsBuffer = (PBTH_LE_GATT_DESCRIPTOR)
			malloc(descriptorBufferSize * sizeof(BTH_LE_GATT_DESCRIPTOR));

		if (NULL == descriptorsBuffer) {
            qCritical() << "\tERROR while allocating space for the descriptors buffer." << endl;
		}
		else {
		RtlZeroMemory(descriptorsBuffer, descriptorBufferSize);
		}

		/* Retrieve the list of descriptors. */
		hr = BluetoothGATTGetDescriptors(
			this->parentService->parentDevice->Handle,
			this,
			descriptorBufferSize,
			descriptorsBuffer,
			&this->numDescriptors,
			BLUETOOTH_GATT_FLAG_NONE);

		if (FAILED(hr)) {
            qCritical() << "\tERROR while getting the list of descriptors: ";
			ErrorDescription(hr);
			return hr;
		}

		if (this->numDescriptors != descriptorBufferSize) {
            qWarning() << "\tWARNING - Mismatch between the size of the buffer and the number of characteristics." << endl;
		}

		/* Create the descriptors. */
		for (int i = 0; i < this->numDescriptors; i++)
			this->descriptors.push_back(new Descriptor(this, &descriptorsBuffer[i]));

		free(descriptorsBuffer);
	}
	
	return hr;
}


/*************************************************************************
*
* Function:		subscribeToNotification()
*
* Description:	Enable the notification and register the callback event
*				for the current characteristic.
*
* Notes:		BluetoothGATTSetDescriptorValue:
*				https://msdn.microsoft.com/en-us/library/windows/hardware/hh450807(v=vs.85).aspx
*
*				BluetoothGATTRegisterEvent:
*				https://msdn.microsoft.com/en-us/library/windows/hardware/hh450804(v=vs.85).aspx
*				The address of the characteristic is passed has the event
*				context.
*
* Parameters:	None.
*
* Returns:		Result of the second call of the
*				BluetoothGATTRegisterEvent.
*
**************************************************************************/
HRESULT Characteristic::subscribeToNotification(__in bool _SubscritbeToNotification, __in bool _SubscribeToIndication, __in PFNBLUETOOTH_GATT_EVENT_CALLBACK _callbackFunction)
{
	/***** Enabling of the notification. *****/
	PBTH_LE_GATT_DESCRIPTOR CCCDescriptor = nullptr;
	BTH_LE_GATT_DESCRIPTOR_VALUE newValue;

	/* Find the CCCD for the current characteristic. */
	for (int i = 0; i < this->numDescriptors; i++) {
		if (this->descriptors[i]->DescriptorType == ClientCharacteristicConfiguration) {
			CCCDescriptor = this->descriptors[i];
			break;
		}
	}

	if (CCCDescriptor == nullptr)
		return ERROR_MONITOR_NO_MORE_DESCRIPTOR_DATA;

	/* Initialize the new value to write to the CCCD. */
	RtlZeroMemory(&newValue, sizeof(newValue));
	newValue.DescriptorType = ClientCharacteristicConfiguration;
	newValue.ClientCharacteristicConfiguration.IsSubscribeToNotification = _SubscritbeToNotification;
	newValue.ClientCharacteristicConfiguration.IsSubscribeToIndication = _SubscribeToIndication;

	/* Write the new value to the CCCD. That enables the notification for the current characteristic. */
	HRESULT hr = BluetoothGATTSetDescriptorValue(
		this->parentService->parentDevice->Handle,
		CCCDescriptor,
		&newValue,
		BLUETOOTH_GATT_FLAG_NONE);

	if (FAILED(hr)) {
        qCritical() << "\tERROR while enabling the notifications on characteristic " << CCCDescriptor->CharacteristicHandle << ": ";
		ErrorDescription(hr);
		return hr;
	}


	/***** Registering of the callback fucntion. *****/
	BTH_LE_GATT_EVENT_TYPE EventType = CharacteristicValueChangedEvent;
	BLUETOOTH_GATT_VALUE_CHANGED_EVENT_REGISTRATION EventParameterIn;

	EventParameterIn.NumCharacteristics = 1;
	EventParameterIn.Characteristics[0] = *this;

	hr = BluetoothGATTRegisterEvent(
		this->parentService->parentDevice->Handle,
		EventType,
		&EventParameterIn,
		_callbackFunction,
        this,
		&this->EventHandle,
		BLUETOOTH_GATT_FLAG_NONE);

	if (FAILED(hr)) {
        qCritical() << "\tERROR while registering the event callback: ";
		ErrorDescription(hr);
		return hr;
	}

    this->notificationEnabled = TRUE;

	return hr;
}


/*************************************************************************
*
* Function:		unsubscribeToNotification()
*
* Description:	Disable the notification and unregister the callback event
*				for the current characteristic.
*
* Notes:		BluetoothGATTSetDescriptorValue:
*				https://msdn.microsoft.com/en-us/library/windows/hardware/hh450807(v=vs.85).aspx
*
*				BluetoothGATTUnregisterEvent:
*				https://msdn.microsoft.com/en-us/library/windows/hardware/hh450809(v=vs.85).aspx
*
* Parameters:	None.
*
* Returns:		Result of the second call of the
*				BluetoothGATTSetDescriptorValue.
*
**************************************************************************/
HRESULT Characteristic::unsubscribeToNotification(void)
{
    /***** Unregistering of the callback fucntion. *****/
    HRESULT hr = BluetoothGATTUnregisterEvent(
        this->EventHandle,
        BLUETOOTH_GATT_FLAG_NONE);

    if (FAILED(hr)) {
        qCritical() << "\tERROR while unregistering the event callback: ";
        ErrorDescription(hr);
        return hr;
    }

	/***** Disabling of the notification. *****/
	PBTH_LE_GATT_DESCRIPTOR CCCDescriptor = nullptr;
	BTH_LE_GATT_DESCRIPTOR_VALUE newValue;

	/* Find the CCCD for the current characteristic. */
	for (int i = 0; i < this->numDescriptors; i++) {
		if (this->descriptors[i]->DescriptorType == ClientCharacteristicConfiguration) {
			CCCDescriptor = this->descriptors[i];
			break;
		}
	}

	if (CCCDescriptor == nullptr)
		return ERROR_MONITOR_NO_MORE_DESCRIPTOR_DATA;

	/* Initialize the new value to write to the CCCD. */
	RtlZeroMemory(&newValue, sizeof(newValue));
	newValue.DescriptorType = ClientCharacteristicConfiguration;
    newValue.ClientCharacteristicConfiguration.IsSubscribeToNotification = FALSE;
    newValue.ClientCharacteristicConfiguration.IsSubscribeToIndication = FALSE;

	/* Write the new value to the CCCD. That disables the notification for the current characteristic. */
    hr = BluetoothGATTSetDescriptorValue(
		this->parentService->parentDevice->Handle,
		CCCDescriptor,
		&newValue,
		BLUETOOTH_GATT_FLAG_NONE);

	if (FAILED(hr)) {
        qCritical() << "\tERROR while disabling the notifications on characteristic " << CCCDescriptor->CharacteristicHandle << ": ";
		ErrorDescription(hr);
		return hr;
	}

    this->notificationEnabled = FALSE;

	return hr;
}


/*************************************************************************
*
* Function:		readValue()
*
* Description:	Read the value of a characteristic.
*
* Notes:		BluetoothGATTGetCharacteristicValue:
*				https://msdn.microsoft.com/en-us/library/windows/hardware/hh450796(v=vs.85).aspx
*
* Parameters:	The address of the variable that will hold the value.
*
* Returns:		Result of the second call of the
*				BluetoothGATTGetCharacteristicValue.
*
**************************************************************************/
HRESULT Characteristic::readValue(PBTH_LE_GATT_CHARACTERISTIC_VALUE *_value)
{
	/* Determine the size of the buffer required. */
	USHORT characteristicValueDataSize;
	HRESULT hr = BluetoothGATTGetCharacteristicValue(
		this->parentService->parentDevice->Handle,
		this,
		0,
		NULL,
		&characteristicValueDataSize,
		BLUETOOTH_GATT_FLAG_NONE);

	if (HRESULT_FROM_WIN32(ERROR_MORE_DATA) != hr) {
        qCritical() << "ERROR while getting the size of the characteristic's value: ";
		ErrorDescription(hr);
		return hr;
	}

	/* Allocate space for the buffer. */
	*_value = (PBTH_LE_GATT_CHARACTERISTIC_VALUE)malloc(characteristicValueDataSize);

	if (NULL == *_value) {
        qCritical() << "ERROR while allocating space for the characteristic's value." << endl;
	}
	else {
		RtlZeroMemory(*_value, characteristicValueDataSize);
	}

	/* Retrieve the characteristic's value. */
	hr = BluetoothGATTGetCharacteristicValue(
		this->parentService->parentDevice->Handle,
		this,
		(ULONG)characteristicValueDataSize,
		*_value,
		NULL,
		BLUETOOTH_GATT_FLAG_NONE);

	if (FAILED(hr)) {
        qCritical() << "ERROR while getting the characteristic's value: ";
		ErrorDescription(hr);
		return hr;
	}

	return hr;
}


/*************************************************************************
*
* Function:		writeValue()
*
* Description:	Read the value of a characteristic.
*
* Notes:		BluetoothGATTSetCharacteristicValue:
*				https://msdn.microsoft.com/en-us/library/windows/hardware/hh450806(v=vs.85).aspx
*
* Parameters:	The address of the variable that holds the value.
*
* Returns:		Result of the second call of the
*				BluetoothGATTSetCharacteristicValue.
*
**************************************************************************/
HRESULT Characteristic::writeValue(PBTH_LE_GATT_CHARACTERISTIC_VALUE _value)
{
	HRESULT hr = BluetoothGATTSetCharacteristicValue(
		this->parentService->parentDevice->Handle,
		this,
		_value,
		NULL,
		BLUETOOTH_GATT_FLAG_NONE);

	if (FAILED(hr)) {
        qCritical() << "ERROR while setting the characteristic's value: ";
		ErrorDescription(hr);
		return hr;
	}

	return hr;
}


/*************************************************************************
*
* Function:		getName()
*
* Description:	Get the name of the characteristic.
*
* Notes:		The name is located within a Descriptor's value.
*
* Parameters:	The address of the variable that holds the value.
*
* Returns:		Result of the call of Descriptor::readValue.
*
**************************************************************************/
HRESULT Characteristic::getName()
{
    HRESULT hr = E_NOINTERFACE;

    for (int i = 0; i < this->numDescriptors; i++) {
        if (this->descriptors[i]->DescriptorType == CharacteristicUserDescription) {
            PBTH_LE_GATT_DESCRIPTOR_VALUE nameValue;
            hr = this->descriptors[i]->readValue(&nameValue);
            for (USHORT j = 0; j < nameValue->DataSize; j++) {
                if (j%2 == 0)
                    this->name += nameValue->Data[j];
            }
        }
    }
    return hr;
}
