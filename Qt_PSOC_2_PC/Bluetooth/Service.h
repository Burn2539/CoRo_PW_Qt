#pragma once

/*****************************************************************************
* Included headers
*****************************************************************************/
#include "modelBLE.h"

class Device;
class Characteristic;

class Service : public BTH_LE_GATT_SERVICE
{
public:
	Service(Device *_device, PBTH_LE_GATT_SERVICE _service);
	~Service();

	/* Address of the device to which the service is from. */
    Device *parentDevice = nullptr;

	/* List of all the characteristics' addresses. */
	std::vector<Characteristic*> characteristics;

	/* Number of characteristics in the vector. */
	USHORT numCharacteristics;

	/* Function that populates the vector. */
	HRESULT retrieveListCharacteristics();
};
