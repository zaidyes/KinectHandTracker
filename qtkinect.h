#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_qtkinect.h"

#include "KinectManager.h"

class qtkinect : public QMainWindow
{
	Q_OBJECT

public:
	qtkinect(QWidget *parent = Q_NULLPTR);

	public slots:
	void addLog(const QString &msg);

	void on_pb_startTracking_clicked();
	void on_pb_stopTracking_clicked();

signals:


private:
	Ui::qtkinectClass m_ui;
	KinectManager m_kinectManager;
};
