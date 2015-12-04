#pragma once

/*****************************************************************************
* Included headers
*****************************************************************************/
#include "modelBLE.h"

class Service;
class Descriptor;
class CapSense;

class Characteristic : public BTH_LE_GATT_CHARACTERISTIC
{
public:
	Characteristic(Service *_service, PBTH_LE_GATT_CHARACTERISTIC _characteristic);
	~Characteristic();

	/* Address of the service to which the characteristic is from. */
    Service *parentService = nullptr;

	/* List of all the descriptors' addresses. */
	std::vector<Descriptor*> descriptors;

	/* Number of descriptors in the vector. */
	USHORT numDescriptors;

	/* Function that populates the vector. */
	HRESULT retrieveListDescriptors();

	/* Subscribe to the notification or indication of the characteristic. */
	HRESULT subscribeToNotification(__in bool _SubscritbeToNotification, __in bool _SubscribeToIndication, __in PFNBLUETOOTH_GATT_EVENT_CALLBACK _callbackFunction);
	HRESULT unsubscribeToNotification(void);
    bool notificationEnabled = false;
	BLUETOOTH_GATT_EVENT_HANDLE EventHandle;

	/* Read or write the characteristic's value. */
	HRESULT readValue(PBTH_LE_GATT_CHARACTERISTIC_VALUE *_value);
	HRESULT writeValue(PBTH_LE_GATT_CHARACTERISTIC_VALUE _value);

    /* Characteristic's name. */
    QString name = "";
    HRESULT getName();
};
