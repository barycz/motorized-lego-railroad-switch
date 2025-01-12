#pragma once

#include <QObject>
#include <QUdpSocket>
#include <QHostAddress>
#include <QTimer>
#include <QList>
#include "device.h"

class DeviceManager : public QObject
{
	Q_OBJECT

public:
	DeviceManager(QObject *parent = nullptr);
	~DeviceManager();

	QList<Device> getDevices() const;

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
