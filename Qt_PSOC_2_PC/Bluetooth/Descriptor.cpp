#include "Descriptor.h"

using namespace std;

/*************************************************************************
*
*	COSNTRUCTOR
*
**************************************************************************/
Descriptor::Descriptor(Characteristic *_characteristic, PBTH_LE_GATT_DESCRIPTOR _descriptor)
{
	parentCharacteristic = _characteristic;

	this->AttributeHandle = _descriptor->AttributeHandle;
	this->CharacteristicHandle = _descriptor->CharacteristicHandle;
	this->DescriptorType = _descriptor->DescriptorType;
	this->DescriptorUuid = _descriptor->DescriptorUuid;
	this->ServiceHandle = _descriptor->ServiceHandle;
}


/*************************************************************************
*
*	DESCRUCTOR
*
**************************************************************************/
Descriptor::~Descriptor()
{
    qDebug() << "Descriptor has been deleted." << endl;
}


/*************************************************************************
*
* Function:		readValue()
*
* Description:	Read the value of a descriptor.
*
* Notes:		BluetoothGATTGetDescriptorValue:
*				https://msdn.microsoft.com/en-us/library/windows/hardware/hh450797(v=vs.85).aspx
*
* Parameters:	The address of the descriptor which it reads the value
*				and the address of the variable that will hold the value.
*
* Returns:		Result of the second call of the
*				BluetoothGATTGetDescriptorValue.
*
**************************************************************************/
HRESULT Descriptor::readValue(PBTH_LE_GATT_DESCRIPTOR_VALUE *_value)
{
	/* Determine the size of the buffer required. */
	USHORT descriptorValueDataSize;
	HRESULT hr = BluetoothGATTGetDescriptorValue(
		this->parentCharacteristic->parentService->parentDevice->Handle,
        this,
		0,
		NULL,
		&descriptorValueDataSize,
		BLUETOOTH_GATT_FLAG_NONE);

	if (HRESULT_FROM_WIN32(ERROR_MORE_DATA) != hr) {
        qCritical() << "\tERROR while getting the size of the descriptor's value: ";
		ErrorDescription(hr);
		return hr;
	}

	/* Allocate space for the buffer. */
	*_value = (PBTH_LE_GATT_DESCRIPTOR_VALUE)malloc(descriptorValueDataSize);

	if (NULL == *_value) {
        qCritical() << "\tERROR while allocating space for the descriptor's value." << endl;
	}
	else {
		RtlZeroMemory(*_value, descriptorValueDataSize);
	}

	/* Retrieve the characteristic's value. */
	hr = BluetoothGATTGetDescriptorValue(
		this->parentCharacteristic->parentService->parentDevice->Handle,
        this,
		(ULONG)descriptorValueDataSize,
		*_value,
		NULL,
		BLUETOOTH_GATT_FLAG_NONE);

	if (FAILED(hr)) {
        qCritical() << "\tERROR while getting the descriptor's value: ";
		ErrorDescription(hr);
		return hr;
	}

	return hr;
}


/*************************************************************************
*
* Function:		writeValue()
*
* Description:	Read the value of a descriptor.
*
* Notes:		BluetoothGATTSetDescriptorValue:
*				https://msdn.microsoft.com/en-us/library/windows/hardware/hh450807(v=vs.85).aspx
*
* Parameters:	The address of the variable that holds the value.
*
* Returns:		Result of the second call of the
*				BluetoothGATTSetDescriptorValue.
*
**************************************************************************/
HRESULT Descriptor::writeValue(PBTH_LE_GATT_DESCRIPTOR_VALUE *_value)
{
	HRESULT hr = BluetoothGATTSetDescriptorValue(
		this->parentCharacteristic->parentService->parentDevice->Handle,
		this,
		*_value,
		BLUETOOTH_GATT_FLAG_NONE);

	if (FAILED(hr)) {
        qCritical() << "ERROR while getting the characteristic's value: ";
		ErrorDescription(hr);
		return hr;
	}

	return hr;
}
