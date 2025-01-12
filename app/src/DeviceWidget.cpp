#include "DeviceWidget.h"

#include "ui_DeviceWidget.h"

#include <QDebug>
#include <QStyle>

DeviceWidget::DeviceWidget(QWidget *parent)
	: QWidget(parent), ui(new Ui::DeviceWidget)
{
	ui->setupUi(this);
	connect(ui->leftButton, &QPushButton::clicked, this, &DeviceWidget::onLeftButtonClicked);
	connect(ui->centerButton, &QPushButton::clicked, this, &DeviceWidget::onCenterButtonClicked);
	connect(ui->rightButton, &QPushButton::clicked, this, &DeviceWidget::onRightButtonClicked);
}

DeviceWidget::~DeviceWidget()
{
	delete ui;
}

void DeviceWidget::setDevice(const Device &device)
{
	currentDevice = &device;
	ui->nameLabel->setText(device.getName());
	ui->addressLabel->setText(device.getAddress().toString());
	if (device.getStatus() == DeviceStatus::Online)
	{
		ui->statusLabel->setPixmap(style()->standardPixmap(QStyle::SP_DialogApplyButton));
	}
	else
	{
		ui->statusLabel->setPixmap(style()->standardPixmap(QStyle::SP_DialogCancelButton));
	}
}

const Device* DeviceWidget::getCurrentDevice() const
{
	return currentDevice;
}

void DeviceWidget::onLeftButtonClicked()
{
	emit buttonClicked(*currentDevice, "left"); // Emit the Device instance
}

void DeviceWidget::onCenterButtonClicked()
{
	emit buttonClicked(*currentDevice, "center"); // Emit the Device instance
}

void DeviceWidget::onRightButtonClicked()
{
	emit buttonClicked(*currentDevice, "right"); // Emit the Device instance
}