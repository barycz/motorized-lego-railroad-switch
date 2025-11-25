
#pragma once

#include "RailwayDevice.h"

#include "lwip/pbuf.h"

#include <array>

namespace RailwayProtocol {

class Packet;

class DeviceManager {
public:
	DeviceManager();

	void OnPacketReceived(const Packet& packet, const ip_addr_t* addr, u16_t port);
	Device* FindOrCreateDevice(const ip_addr_t* addr, u16_t port);
	size_t GetDeviceCount() const;

	template<typename Fn>
	void ForEachDevice(Fn fn) const {
		for (auto& device : Devices) {
			if (IsValidDevice(device)) {
				fn(device);
			}
		}
	}

private:
	static bool IsValidDevice(const Device& device);
	void HandlePacket(const Packet& packet, Device& device);

	std::array<Device, 16> Devices;
};

}