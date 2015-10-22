#include "Device.h"

using namespace std;

/*************************************************************************
*
*	COSNTRUCTOR
*
**************************************************************************/
Device::Device(GUID _guid)
{
	this->Guid = _guid;
	this->Handle = getHandle(Guid);
}


/*************************************************************************
*
*	DESCTRUCTOR
*
**************************************************************************/
Device::~Device()
{
	for (int i = 0; i < this->numServices; i++)
		delete this->services[i];
	CloseHandle(this->Handle);
    qDebug() << "Device has been deleted." << endl;
}


/*************************************************************************
*
* Function:		retrieveListServices()
*
* Description:	Retrieve the list of all the services available on the
*				BLE device.
*
* Notes:		Must have called getDeviceHandle before.
*
*				First send 0,NULL as the parameters to
*				BluetoothGATTServices in order to get the number of
*				services in servicesBufferCount.
*
*				BluetoothGATTGetServices:
*				https://msdn.microsoft.com/en-us/library/windows/hardware/hh450802(v=vs.85).aspx
*
* Parameters:	None.
*
* Returns:		Result of the second call of the BluetoothGATTGetServices.
*
**************************************************************************/
HRESULT Device::retrieveListServices()
{
	PBTH_LE_GATT_SERVICE servicesBuffer;
	USHORT serviceBufferSize;

	/* Determine the size of the buffer required. */
	HRESULT hr = BluetoothGATTGetServices(
		this->Handle,
		0,
		NULL,
		&serviceBufferSize,
		BLUETOOTH_GATT_FLAG_NONE);

	if (HRESULT_FROM_WIN32(ERROR_MORE_DATA) != hr) {
        qCritical() << "ERROR while getting the size of the services buffer: ";
		ErrorDescription(hr);
		return hr;
	}

	/* Allocate space for the buffer. */
	if (serviceBufferSize > 0)
	{
		servicesBuffer = (PBTH_LE_GATT_SERVICE)
			malloc(sizeof(BTH_LE_GATT_SERVICE) * serviceBufferSize);

		if (NULL == servicesBuffer) {
            qCritical() << "ERROR while allocating space for the services buffer." << endl;
		}
		else {
			RtlZeroMemory(servicesBuffer, sizeof(BTH_LE_GATT_SERVICE) * serviceBufferSize);
		}

		/* Retrieve the list of services. */
		hr = BluetoothGATTGetServices(
			this->Handle,
			serviceBufferSize,
			servicesBuffer,
			&this->numServices,
			BLUETOOTH_GATT_FLAG_NONE);

		if (FAILED(hr)) {
            qCritical() << "ERROR while getting the list of services: ";
			ErrorDescription(hr);
			return hr;
		}

		/* Create the services. */
		for (int i = 0; i < this->numServices; i++)
			this->services.push_back( new Service( this, &servicesBuffer[i] ) );

		free(servicesBuffer);
	}

	return hr;
}


/*************************************************************************
*
* Function:		getHandle()
*
* Description:	Get the device handle from its GUID.
*
* Notes:		Copied from:
*				http://social.msdn.microsoft.com/Forums/windowshardware/en-US/e5e1058d-5a64-4e60-b8e2-0ce327c13058/erroraccessdenied-error-when-trying-to-receive-data-from-bluetooth-low-energy-devices?forum=wdk
*
* Parameters:	The GUID of the device.
*
* Returns:		Handle of the device.
*
**************************************************************************/
HANDLE Device::getHandle(GUID AGuid)
{
	HDEVINFO hDI;
	SP_DEVICE_INTERFACE_DATA did;
	SP_DEVINFO_DATA dd;
	GUID BluetoothInterfaceGUID = AGuid;
	HANDLE hComm = NULL;

	hDI = SetupDiGetClassDevs(&BluetoothInterfaceGUID, NULL, NULL, DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);

	if (hDI == INVALID_HANDLE_VALUE) return NULL;

	did.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
	dd.cbSize = sizeof(SP_DEVINFO_DATA);

	for (DWORD i = 0; SetupDiEnumDeviceInterfaces(hDI, NULL, &BluetoothInterfaceGUID, i, &did); i++)
	{
		SP_DEVICE_INTERFACE_DETAIL_DATA DeviceInterfaceDetailData;

		DeviceInterfaceDetailData.cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

		DWORD size = 0;

		if (!SetupDiGetDeviceInterfaceDetail(hDI, &did, NULL, 0, &size, 0))
		{
			int err = GetLastError();

			if (err == ERROR_NO_MORE_ITEMS) break;

			PSP_DEVICE_INTERFACE_DETAIL_DATA pInterfaceDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)GlobalAlloc(GPTR, size);

			pInterfaceDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

			if (!SetupDiGetDeviceInterfaceDetail(hDI, &did, pInterfaceDetailData, size, &size, &dd))
				break;

			hComm = CreateFile(
				pInterfaceDetailData->DevicePath,
				GENERIC_WRITE | GENERIC_READ,
				FILE_SHARE_READ | FILE_SHARE_WRITE,
				NULL,
				CREATE_ALWAYS,
				0,
				NULL);

			GlobalFree(pInterfaceDetailData);
		}
	}

	SetupDiDestroyDeviceInfoList(hDI);
	return hComm;
}
