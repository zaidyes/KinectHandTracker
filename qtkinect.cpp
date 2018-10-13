#include "qtkinect.h"

#include <NetworkHandler.h>

qtkinect::qtkinect(QWidget *parent)
	: QMainWindow(parent)
{
	qRegisterMetaType<Commands>("Commands");
	m_ui.setupUi(this);
	connect(&m_kinectManager, &KinectManager::log, this, &qtkinect::addLog);
}


void qtkinect::addLog(const QString &msg)
{
	QPlainTextEdit *logTextEdit = m_ui.pte_logWindow;
	logTextEdit->appendPlainText(msg);
}

void qtkinect::on_pb_startTracking_clicked()
{
	NetworkConfig networkConfig;
	networkConfig.activate		= m_ui.cb_broadcast->isChecked();
	networkConfig.ipAddress		= m_ui.le_ipaddress->text();
	networkConfig.port			= m_ui.sb_port->value();
	m_kinectManager.setNetworkConfig(networkConfig);

	m_kinectManager.startKinect();
}

void qtkinect::on_pb_stopTracking_clicked()
{
	m_kinectManager.stopKinect();
}
