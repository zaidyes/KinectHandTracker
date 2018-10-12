#include "KinectManager.h"
#include <QApplication>


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
	if (m_bodyTracker)
		return;

	m_bodyTracker = new BodyTracker();
	m_bodyTracker->moveToThread(&m_workerThread);
	
	connect(m_bodyTracker, &BodyTracker::newLog, this, &KinectManager::onMessage);

	connect(this, &KinectManager::stop, m_bodyTracker, &BodyTracker::onStopRequest);
	connect(m_bodyTracker, &BodyTracker::destroyed, [this]() { m_bodyTracker = nullptr;});
	connect(&m_workerThread, &QThread::finished, m_bodyTracker, &QObject::deleteLater);

	connect(&m_workerThread, &QThread::started, m_bodyTracker, &BodyTracker::onStartRequest);
	m_workerThread.start();
	emit onMessage("Thread started");
}

void KinectManager::stopKinect()
{
	emit onMessage("Stopping thread");
	emit stop();
	m_workerThread.exit();
}
