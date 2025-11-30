
#include "RailwayDeviceController.h"
#include "RailwayDeviceManager.h"
#include "RailwayLwIPPacketSender.h"

#include "Ui.h"

#include "lwip/ip_addr.h"

void RailwayDeviceController::Update() {
	Ui::BeginWidget();
	Ui::Text("Devices:");
	DeviceManager.ForEachDevice([&](const RailwayProtocol::Device& device) {
		UpdateDevice(device);
	});
	Ui::EndWidget();
}

void RailwayDeviceController::UpdateDevice(const RailwayProtocol::Device& device) {
	const char dir
		= device.SwitchDirection == RailwayProtocol::ESwitchDirection::Left ? 'L'
		: device.SwitchDirection == RailwayProtocol::ESwitchDirection::Right ? 'R'
		: 'C';
	Ui::Text("%s [%s] %c", device.Name, ipaddr_ntoa(&device.Address), dir);
	if (Ui::IsButtonPressed(Ui::Button::X)) {
		ToggleSwitch(const_cast<RailwayProtocol::Device&>(device));
	}
}

void RailwayDeviceController::ToggleSwitch(RailwayProtocol::Device& device) {
	device.SwitchDirection
		= device.SwitchDirection == RailwayProtocol::ESwitchDirection::Left
		? RailwayProtocol::ESwitchDirection::Right
		: RailwayProtocol::ESwitchDirection::Left;
	RailwayProtocol::LwIPPacketSender::SendSetSwitch(*Udp, device.Address, device.SwitchDirection);
}
