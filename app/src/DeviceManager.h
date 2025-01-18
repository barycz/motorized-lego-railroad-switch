#pragma once

#include "Device.h"

#include <QObject>
#include <QUdpSocket>
#include <QHostAddress>
#include <QTimer>
#include <QList>

class DeviceManager : public QObject
{
	Q_OBJECT

public:
	DeviceManager(QObject *parent = nullptr);
	~DeviceManager();

	QList<Device> getDevices() const;
	void sendCommand(const Device &device, char command);

signals:
	void devicesUpdated(const QList<Device> &devices);

private slots:
	void processPendingDatagrams();
	void cleanupInactiveHosts();

private:
	QUdpSocket *udpSocket;
	const quint16 UDP_PORT = 57890;
	QList<Device> devices;
	QTimer *cleanupTimer;
	const int INACTIVE_TIMEOUT_MS = 10000;
};
