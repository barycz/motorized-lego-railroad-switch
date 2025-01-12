#pragma once

#include <QHostAddress>
#include <QDateTime>
#include <QString>

enum class DeviceStatus
{
	Online,
	Offline
};

class Device
{
public:
	Device(const QHostAddress &address, const QString &name, quint16 port)
		: address(address), name(name), port(port), status(DeviceStatus::Online), lastSeen(QDateTime::currentDateTime()) {}

	QHostAddress getAddress() const { return address; }
	QString getName() const { return name; }
	void setName(const QString &name) { this->name = name; }
	quint16 getPort() const { return port; }
	void setPort(quint16 port) { this->port = port; }
	DeviceStatus getStatus() const { return status; }
	void setStatus(DeviceStatus status) { this->status = status; }
	QDateTime getLastSeen() const { return lastSeen; }
	void setLastSeen(const QDateTime &lastSeen) { this->lastSeen = lastSeen; }

private:
	QHostAddress address;
	QString name;
	quint16 port;
	DeviceStatus status;
	QDateTime lastSeen;
};
