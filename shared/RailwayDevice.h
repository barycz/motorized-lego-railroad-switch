
#pragma once

#include "RailwayProtocol.h"
#include "ReplicatedVariable.h"

#include "lwip/ip_addr.h"

namespace RailwayProtocol {

struct Device {
	static const size_t MaxDeviceName = 16;

	ip_addr_t Address;
	u16_t Port;
	ReplicatedVariable<ESwitchDirection> SwitchDirection;
	char Name[MaxDeviceName];
};

}
