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

struct Packet {
	enum class EMsgType: uint8_t {
		Beacon = 1,
	};

	static size_t NewBeacon(void* buffer, size_t bufferSize, const char* deviceName);

	EMagic Magic;
	EVersion Version;
	EMsgType MsgType;
	uint8_t Data[];
};

static_assert(sizeof(Packet) == 3);

}