#include "MainWindow.h"
#include "DeviceWidget.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QListWidgetItem>

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
	ui->hostListWidget->clear();
	for (const auto &device : devices)
	{
		DeviceWidget *deviceWidget = new DeviceWidget(ui->hostListWidget); // Create a new widget
		deviceWidget->setDevice(device);

		QListWidgetItem *item = new QListWidgetItem();
		item->setSizeHint(deviceWidget->sizeHint()); // Set item size to widget size
		ui->hostListWidget->addItem(item);
		ui->hostListWidget->setItemWidget(item, deviceWidget); // Set the widget for the item
	}
}
