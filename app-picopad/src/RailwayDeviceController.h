
#pragma once

struct udp_pcb;

namespace RailwayProtocol {
	struct Device;
	class DeviceManager;
}

class RailwayDeviceController {
public:
	RailwayDeviceController(RailwayProtocol::DeviceManager& deviceManager, udp_pcb*& udp)
		: DeviceManager(deviceManager), Udp(udp) {}
	~RailwayDeviceController() {}

	void Update();

private:
	void UpdateDevice(const RailwayProtocol::Device& device);

	RailwayProtocol::DeviceManager& DeviceManager;
	udp_pcb*& Udp;
};
