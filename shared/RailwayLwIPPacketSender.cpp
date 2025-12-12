
#include "RailwayLwIPPacketSender.h"

#include "RailwayProtocol.h"

#include "lwip/udp.h"

#include <stdio.h>
#include <string.h>

namespace RailwayProtocol {

void LwIPPacketSender::SendBeacon(udp_pcb& udp, const char* deviceName) {
	static const size_t BeaconSize = 32;
	uint8_t buffer[BeaconSize];
	const size_t packetSize = RailwayProtocol::Packet::NewBeacon(buffer, BeaconSize, deviceName);
	struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, packetSize, PBUF_RAM);
	memcpy(p->payload, buffer, packetSize);
	const err_t er = udp_sendto(&udp, p, IP_ADDR_BROADCAST, RailwayProtocol::UdpPort);
	if (er != ERR_OK) {
		printf("Failed to send beacon.\n");
	}
	pbuf_free(p);
}

void LwIPPacketSender::SendStatus(udp_pcb& udp, const ip_addr_t& addr, ESwitchDirection dir) {
	static const size_t StatusSize = 32;
	uint8_t buffer[StatusSize];
	const size_t packetSize = RailwayProtocol::Packet::NewStatus(buffer, StatusSize, dir);
	struct pbuf* p = pbuf_alloc(PBUF_TRANSPORT, packetSize, PBUF_RAM);
	memcpy(p->payload, buffer, packetSize);
	const err_t er = udp_sendto(&udp, p, &addr, RailwayProtocol::UdpPort);
	if (er != ERR_OK) {
		printf("Failed to send beacon.\n");
	}
	pbuf_free(p);
}

void LwIPPacketSender::SendSetSwitch(udp_pcb& udp, const ip_addr_t& addr, ESwitchDirection dir) {
	static const size_t SetSwitchSize = 32;
	uint8_t buffer[SetSwitchSize];
	const size_t packetSize = RailwayProtocol::Packet::NewSetSwitch(buffer, SetSwitchSize, dir);
	struct pbuf* p = pbuf_alloc(PBUF_TRANSPORT, packetSize, PBUF_RAM);
	memcpy(p->payload, buffer, packetSize);
	const err_t er = udp_sendto(&udp, p, &addr, RailwayProtocol::UdpPort);
	if (er != ERR_OK) {
		printf("Failed to send beacon.\n");
	}
	pbuf_free(p);
}

}