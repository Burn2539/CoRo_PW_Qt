#pragma once

/*****************************************************************************
* Included headers
*****************************************************************************/
#include "modelBLE.h"

class Service;

class Device
{
public:
	Device(GUID _guid);
	~Device();

	/* Device's GUID. */
	GUID Guid;

	/* Handle to the BLE device to wich you are connected. */
    HANDLE Handle;
	HANDLE getHandle(GUID AGuid);

	/* List of all the services' addresses. */
	std::vector<Service*> services;

	/* Number of services in the vector. */
	USHORT numServices;

	/* Function that populates the vector. */
	HRESULT retrieveListServices();
};
