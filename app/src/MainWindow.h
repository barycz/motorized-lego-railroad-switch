#pragma once

#include "DeviceManager.h"

#include <QMainWindow>
#include <QIcon>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class DeviceWidget;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = nullptr);
	~MainWindow();

private slots:
	void updateDeviceList(const QList<Device> &devices);
	void handleButtonClicked(const Device& device, const QString& button);

private:
	DeviceWidget* getOrCreateWidget(const Device& device);

	Ui::MainWindow *ui;
	DeviceManager deviceManager;
	QIcon windowIcon;
};
