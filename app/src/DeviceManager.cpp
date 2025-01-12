#include "DeviceManager.h"

#include <QDebug>
#include <QDateTime>

DeviceManager::DeviceManager(QObject *parent)
		: QObject(parent), udpSocket(new QUdpSocket(this))
{
	connect(udpSocket, &QUdpSocket::readyRead, this, &DeviceManager::processPendingDatagrams);

	if (!udpSocket->bind(QHostAddress::AnyIPv4, UDP_PORT))
	{
		qDebug() << "Error binding to port:" << udpSocket->errorString();
		return;
	}

	cleanupTimer = new QTimer(this);
	connect(cleanupTimer, &QTimer::timeout, this, &DeviceManager::cleanupInactiveHosts);
	cleanupTimer->start(1000);
}

DeviceManager::~DeviceManager()
{
	udpSocket->close();
}

QList<Device> DeviceManager::getDevices() const
{
	return devices;
}

void DeviceManager::sendCommand(const Device &device, const QString &command)
{
	QByteArray datagram = command.toUtf8();
	udpSocket->writeDatagram(datagram, device.getAddress(), device.getPort()); // Use device's port
	qDebug() << "Sent command '" << command << "' to" << device.getAddress().toString() << ":" << device.getPort();
}

void DeviceManager::processPendingDatagrams()
{
	while (udpSocket->hasPendingDatagrams())
	{
		QByteArray datagram;
		datagram.resize(udpSocket->pendingDatagramSize());
		QHostAddress sender;
		quint16 senderPort;

		udpSocket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);

		if (datagram.size() < 3) // TODO: parser
		{
			qDebug() << "Received too short datagram from" << sender.toString();
			continue;
		}

		QByteArray nameBytes = datagram.mid(3);
		QString deviceName = QString::fromLocal8Bit(nameBytes);

		if (deviceName.isEmpty())
		{
			qDebug() << "Received empty or non-UTF8 datagram from" << sender.toString();
			continue; // Skip processing if the name is invalid
		}

		bool found = false;
		for (auto &device : devices)
		{
			if (device.getAddress() == sender)
			{
				device.setLastSeen(QDateTime::currentDateTime());
				device.setStatus(DeviceStatus::Online);
				device.setName(deviceName);
				device.setPort(senderPort);
				found = true;
				break;
			}
		}
		if (!found)
		{
			devices.append(Device(sender, deviceName, senderPort));
		}

		emit devicesUpdated(devices);
		qDebug() << "Beacon from" << sender.toString() << ":" << senderPort << ", Name: " << deviceName;
	}
}

void DeviceManager::cleanupInactiveHosts()
{
	QDateTime now = QDateTime::currentDateTime();

	for (auto &device : devices)
	{
		if (device.getLastSeen().msecsTo(now) > INACTIVE_TIMEOUT_MS)
		{
			device.setStatus(DeviceStatus::Offline);
		}
		else
		{
			device.setStatus(DeviceStatus::Online);
		}
	}
	emit devicesUpdated(devices);
}
