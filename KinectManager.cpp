#include "KinectManager.h"
#include <globals.h>


KinectManager::KinectManager(QObject *parent)
	: QObject(parent)
{
}


KinectManager::~KinectManager()
{
	stopKinect();
}

void KinectManager::startKinect()
{
	//Get the default Kinect sensor
	HRESULT hr = GetDefaultKinectSensor(&m_kinectSensor);

	//If the function succeeds, open the sensor
	if (SUCCEEDED(hr)) {
		hr = m_kinectSensor->Open();
		emit log("DEBUG:Sensor started");
		trackBody();
		return;
	}

	if (m_kinectSensor == nullptr || FAILED(hr)) {
		emit log("ERROR:Cannot find any sensors.");
	}
}

void KinectManager::stopKinect()
{
	emit log("DEBUG:Stopping trackers");
	emit stop();
	m_workerThread.exit();

	emit log("DEBUG:Stopping kinect");
	if (m_kinectSensor) {
		m_kinectSensor->Close();
		safeRelease(m_kinectSensor);
	}
}

void KinectManager::trackBody()
{
	if (m_bodyTracker || !m_kinectSensor)
		return;

	m_bodyTracker = new BodyTracker();
	m_bodyTracker->setSensor(m_kinectSensor);
	m_bodyTracker->moveToThread(&m_workerThread);

	connect(m_bodyTracker, &BodyTracker::command, &m_networkHandler, &NetworkHandler::onCommand);
	connect(&m_networkHandler, &NetworkHandler::log, this, &KinectManager::log);

	connect(m_bodyTracker, &BodyTracker::log, this, &KinectManager::log, Qt::QueuedConnection);
	connect(this, &KinectManager::stop, m_bodyTracker, &BodyTracker::onStopRequest, Qt::QueuedConnection);
	connect(m_bodyTracker, &BodyTracker::destroyed, [this]() { m_bodyTracker = nullptr; });
	connect(&m_workerThread, &QThread::finished, m_bodyTracker, &QObject::deleteLater, Qt::QueuedConnection);

	connect(&m_workerThread, &QThread::started, m_bodyTracker, &BodyTracker::onStartRequest, Qt::QueuedConnection);

	m_workerThread.start();
}

void KinectManager::setNetworkConfig(const NetworkConfig & config)
{
	m_networkHandler.setConfig(config);
}
