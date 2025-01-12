#include "DeviceWidget.h"

#include "ui_DeviceWidget.h"

#include <QDebug>
#include <QStyle>

DeviceWidget::DeviceWidget(QWidget *parent)
	: QWidget(parent), ui(new Ui::DeviceWidget)
{
	ui->setupUi(this);
}

DeviceWidget::~DeviceWidget()
{
	delete ui;
}

void DeviceWidget::setDevice(const Device &device)
{
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
