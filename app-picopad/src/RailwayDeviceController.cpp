
#include "RailwayDeviceController.h"
#include "RailwayDeviceManager.h"
#include "RailwayLwIPPacketSender.h"

#include "Ui.h"

#include "lwip/ip_addr.h"

void RailwayDeviceController::Update() {
	Ui::BeginWidget();
	Ui::TextUnformatted("Devices:");
	Ui::SetColor(Ui::Color::Green());
	DeviceManager.ForEachDevice([&](const RailwayProtocol::Device& device) {
		UpdateDevice(device);
	});
	Ui::EndWidget();
}

void RailwayDeviceController::UpdateDevice(const RailwayProtocol::Device& device) {
	if (Ui::IsButtonPressed(Ui::Button::X)) {
		ToggleSwitch(const_cast<RailwayProtocol::Device&>(device));
	}
	const char localDir = RailwayProtocol::ToChar(device.SwitchDirection.GetLocal());
	const char remoteDir = RailwayProtocol::ToChar(device.SwitchDirection.GetRemote());
	if (localDir != remoteDir) {
		Ui::Text("%s [%s] %c -> %c", device.Name, ipaddr_ntoa(&device.Address), remoteDir, localDir);
		RailwayProtocol::LwIPPacketSender::SendSetSwitch(*Udp, device.Address, device.SwitchDirection.GetLocal());
	} else {
		Ui::Text("%s [%s] %c", device.Name, ipaddr_ntoa(&device.Address), localDir);
	}
}

void RailwayDeviceController::ToggleSwitch(RailwayProtocol::Device& device) {
	device.SwitchDirection.SetLocal(
		device.SwitchDirection.GetLocal() == RailwayProtocol::ESwitchDirection::Left
		? RailwayProtocol::ESwitchDirection::Right
		: RailwayProtocol::ESwitchDirection::Left);
}
