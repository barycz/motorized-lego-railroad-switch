#pragma once

#include "device.h"

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class DeviceWidget; }
QT_END_NAMESPACE

class DeviceWidget : public QWidget
{
	Q_OBJECT

public:
	explicit DeviceWidget(QWidget *parent = nullptr);
	~DeviceWidget();

	void setDevice(const Device& device);

private:
	Ui::DeviceWidget *ui;
};
