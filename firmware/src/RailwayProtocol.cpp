#include "RailwayProtocol.h"

#include <string.h>
#include <stdio.h>

namespace RailwayProtocol {

size_t Packet::NewBeacon(void* buffer, size_t bufferSize, const char* deviceName) {
	Packet* packet = static_cast<Packet*>(buffer);
	packet->Magic = EMagic::Magic;
	packet->Version = EVersion::V1;
	packet->MsgType = EMsgType::Beacon;
	const size_t allowedDataSize = bufferSize - sizeof(*packet);
	const size_t deviceNameLen = strlen(deviceName);
	const size_t toCopyLen = deviceNameLen < allowedDataSize ? deviceNameLen : allowedDataSize - 1;
	memset(packet->Data, 0, allowedDataSize);
	memcpy(packet->Data, deviceName, toCopyLen);
	return sizeof(*packet) + toCopyLen + 1;
}

Packet* Packet::FromBuffer(void* buffer, size_t bufferSize) {
	// for simplicity now expect always at least one byte of data after the header
	if (bufferSize < PacketSize + 1) {
		printf("received %u bytes, insufficient packet size\n", bufferSize);
		return nullptr;
	}

	Packet* ret = static_cast<Packet*>(buffer);
	return ret;
}

}