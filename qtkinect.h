#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_qtkinect.h"

class qtkinect : public QMainWindow
{
	Q_OBJECT

public:
	qtkinect(QWidget *parent = Q_NULLPTR);

	public slots:
	void addLog(const QString &msg);

	void on_pb_startTracking_clicked();

signals:
	void startTracking();
	void stopTracking();


private:
	Ui::qtkinectClass m_ui;
};
