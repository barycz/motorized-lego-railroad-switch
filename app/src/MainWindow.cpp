#include "MainWindow.h"
#include "DeviceWidget.h"
#include "ui_mainwindow.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
		: QMainWindow(parent), ui(new Ui::MainWindow), deviceManager(this)
{
	ui->setupUi(this);
	connect(&deviceManager, &DeviceManager::devicesUpdated, this, &MainWindow::updateDeviceList);
	windowIcon.addFile(":/switch.png");
	setWindowIcon(windowIcon);
	setWindowTitle("Device Manager");
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::updateDeviceList(const QList<Device> &devices)
{
	for (const auto &device : devices)
	{
		auto* deviceWidget = getOrCreateWidget(device);
		if (deviceWidget)
		{
			deviceWidget->setDevice(device); // set the device to refresh the ui
		}
	}
}

DeviceWidget* MainWindow::getOrCreateWidget(const Device& device)
{
	for (int i = 0; i < ui->hostListWidget->count(); ++i)
	{
		QListWidgetItem* item = ui->hostListWidget->item(i);
		if (item)
		{
			QWidget* widget = ui->hostListWidget->itemWidget(item);
			DeviceWidget* dw = qobject_cast<DeviceWidget*>(widget);
			if (dw && dw->getCurrentDevice() == &device)
			{
				return dw;
			}
		}
	}

	DeviceWidget *deviceWidget = new DeviceWidget(ui->hostListWidget);
	deviceWidget->setDevice(device);
	connect(deviceWidget, &DeviceWidget::buttonClicked, this, &MainWindow::handleButtonClicked);

	QListWidgetItem *item = new QListWidgetItem();
	item->setSizeHint(deviceWidget->sizeHint());
	ui->hostListWidget->addItem(item);
	ui->hostListWidget->setItemWidget(item, deviceWidget);
	return deviceWidget;
}

void MainWindow::handleButtonClicked(const Device &device, const QString &button)
{
	if (button == "center")
	{
		deviceManager.sendCommand(device, 0);
	}
	else if (button == "left")
	{
		deviceManager.sendCommand(device, 1);
	}
	else
	{
		deviceManager.sendCommand(device, 2);
	}
}