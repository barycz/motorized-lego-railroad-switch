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
	ui->hostListWidget->clear();
	for (const auto &device : devices)
	{
		DeviceWidget *deviceWidget = new DeviceWidget(ui->hostListWidget);
		deviceWidget->setDevice(device);
		connect(deviceWidget, &DeviceWidget::buttonClicked, this, &MainWindow::handleButtonClicked);

		QListWidgetItem *item = new QListWidgetItem();
		item->setSizeHint(deviceWidget->sizeHint());
		ui->hostListWidget->addItem(item);
		ui->hostListWidget->setItemWidget(item, deviceWidget);
	}
}

void MainWindow::handleButtonClicked(const Device &device, const QString &button)
{
	deviceManager.sendCommand(device, button); // Pass the Device instance to DeviceManager
}