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

	void setDevice(const Device &device);
	const Device *getCurrentDevice() const; // Add getter for the current device

signals:
	void buttonClicked(const Device &device, const QString &button); // Emit Device instance

private slots:
	void onLeftButtonClicked();
	void onCenterButtonClicked();
	void onRightButtonClicked();

private:
	Ui::DeviceWidget *ui;
	const Device *currentDevice;
};
