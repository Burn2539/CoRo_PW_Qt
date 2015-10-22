#pragma once
#include "modelBLE.h"

class Characteristic;

class Descriptor : public BTH_LE_GATT_DESCRIPTOR
{
public:
	Descriptor(Characteristic *_characteristic, PBTH_LE_GATT_DESCRIPTOR _descriptor);
	~Descriptor();

	/* Address of the characteristic to which the descriptor is from. */
	Characteristic *parentCharacteristic;

	/* Read or write the descriptor's value. */
    HRESULT readValue(PBTH_LE_GATT_DESCRIPTOR_VALUE *_value);
	HRESULT writeValue(PBTH_LE_GATT_DESCRIPTOR_VALUE *_value);
};

