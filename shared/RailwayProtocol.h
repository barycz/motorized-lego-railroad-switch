#pragma once

#include <stdint.h>
#include <stddef.h>

namespace RailwayProtocol {

enum class EMagic: uint8_t {
	Magic = 'R',
};

enum class EVersion: uint8_t {
	V1 = 1,
};

enum class ESwitchDirection: uint8_t {
	Center = 0, // invalid position, used for servo calibration
	Left = 1,
	Right = 2,
};

struct Packet {
	enum class EMsgType: uint8_t {
		Beacon = 1,
		SetSwitch = 2,
		Status = 3,
	};

	static size_t NewBeacon(void* buffer, size_t bufferSize, const char* deviceName);
	static size_t NewStatus(void* buffer, size_t bufferSize, ESwitchDirection dir);
	static size_t NewSetSwitch(void* buffer, size_t bufferSize, ESwitchDirection dir);
	static Packet* FromBuffer(void* buffer, size_t bufferSize);

	EMagic Magic;
	EVersion Version;
	EMsgType MsgType;
	uint8_t Data[];
};

static constexpr auto PacketSize = sizeof(Packet);
static_assert(PacketSize == 3);

const uint16_t UdpPort = 57890;
const uint32_t BeaconIntervalUs = 1000000;
const uint32_t StatusIntervalUs = 500000;

char ToChar(ESwitchDirection direction);

}