
#include "RailwayDeviceManager.h"
#include "RailwayProtocol.h"

#include <cstring>

RailwayProtocol::DeviceManager::DeviceManager() {
	for (auto& device : Devices) {
		ip_addr_set_zero(&device.Address); // must follow IsValidDevice() logic
	}
}

bool RailwayProtocol::DeviceManager::IsValidDevice(const Device& device) {
	ip_addr_t zero;
	ip_addr_set_zero(&zero);
	return ip_addr_eq(&device.Address, &zero) == false;
}

void RailwayProtocol::DeviceManager::OnPacketReceived(const Packet& packet, const ip_addr_t* addr, u16_t port) {
	Device* device = FindOrCreateDevice(addr, port);
	if (device == nullptr) {
		return;
	}
	HandlePacket(packet, *device);
}

RailwayProtocol::Device* RailwayProtocol::DeviceManager::FindOrCreateDevice(const ip_addr_t* addr, u16_t port) {
	ip_addr_t zero;
	ip_addr_set_zero(&zero);
	if (ip_addr_isany(addr) || ip_addr_eq(addr, &zero)) {
		return nullptr;
	}
	for (auto& device : Devices) {
		if (ip_addr_eq(&device.Address, addr)) { // found existing
			return &device;
		}
		if (IsValidDevice(device) == false) { // found empty slot, create new
			ip_addr_copy(device.Address, *addr);
			device.Port = port;
			return &device;
		}
	}
	// no empty slots found
	return nullptr;
}

size_t RailwayProtocol::DeviceManager::GetDeviceCount() const {
	size_t count = 0;
	for (const auto& device : Devices) {
		if (IsValidDevice(device)) {
			count++;
		}
	}
	return count;
}

void RailwayProtocol::DeviceManager::HandlePacket(const Packet& packet, Device& device) {
	if (packet.MsgType == Packet::EMsgType::Beacon) {
		strncpy(device.Name, reinterpret_cast<const char*>(packet.Data), Device::MaxDeviceName);
	}
}
