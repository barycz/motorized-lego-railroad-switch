
#pragma once

#include "RailwayProtocol.h"

#include "lwip/ip_addr.h"

struct udp_pcb;

namespace RailwayProtocol {

struct LwIPPacketSender {
	static void SendBeacon(udp_pcb& udp, const char* deviceName);
	static void SendStatus(udp_pcb& udp, ESwitchDirection dir);
	static void SendSetSwitch(udp_pcb& udp, const ip_addr_t& addr, ESwitchDirection dir);
};

}