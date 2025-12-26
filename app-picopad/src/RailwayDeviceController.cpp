
#include "RailwayDeviceController.h"
#include "RailwayDeviceManager.h"
#include "RailwayLwIPPacketSender.h"

#include "Ui.h"

#include "lwip/ip_addr.h"

void RailwayDeviceController::Update() {
	Ui::BeginWidget();
	Ui::TextUnformatted("Devices:");
	Ui::BeginList(LastActiveDeviceIndex);
	DeviceManager.ForEachDevice([&](const RailwayProtocol::Device& device) {
		UpdateDevice(device);
	});
	Ui::EndList();
	Ui::EndWidget();
}

void RailwayDeviceController::UpdateDevice(const RailwayProtocol::Device& device) {
	const char localDir = RailwayProtocol::ToChar(device.SwitchDirection.GetLocal());
	const char remoteDir = RailwayProtocol::ToChar(device.SwitchDirection.GetRemote());
	if (localDir != remoteDir) {
		Ui::ListItem("%s [%s] %c -> %c", device.Name, ipaddr_ntoa(&device.Address), remoteDir, localDir);
		RailwayProtocol::LwIPPacketSender::SendSetSwitch(*Udp, device.Address, device.SwitchDirection.GetLocal());
	} else {
		if (Ui::ListItem("%s [%s] %c", device.Name, ipaddr_ntoa(&device.Address), localDir)) {
			if (Ui::IsButtonPressed(Ui::Button::X)) {
				ToggleSwitch(const_cast<RailwayProtocol::Device&>(device));
			}
		}
	}
}

void RailwayDeviceController::ToggleSwitch(RailwayProtocol::Device& device) {
	device.SwitchDirection.SetLocal(
		device.SwitchDirection.GetLocal() == RailwayProtocol::ESwitchDirection::Left
		? RailwayProtocol::ESwitchDirection::Right
		: RailwayProtocol::ESwitchDirection::Left);
}
