#include "MainWindow.h"
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
		QString itemText = device.getName() + " (" + device.getAddress().toString() + ")"; // Display name and address
		QListWidgetItem *item = new QListWidgetItem(itemText);
		if (device.getStatus() == DeviceStatus::Online)
		{
			item->setIcon(style()->standardIcon(QStyle::SP_DialogApplyButton));
		}
		else
		{
			item->setIcon(style()->standardIcon(QStyle::SP_DialogCancelButton));
		}
		ui->hostListWidget->addItem(item);
	}
}
