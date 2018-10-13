#pragma once

#include <QObject>
#include <QThread>

#include <Kinect.h>

#include <BodyTracker.h>
#include <NetworkHandler.h>

struct NetworkConfig;

class KinectManager : public QObject
{
	Q_OBJECT

public:
	KinectManager(QObject *parent = 0);
	~KinectManager();

	void startKinect();
	void stopKinect();

	void trackBody();

	void setNetworkConfig(const NetworkConfig &config);

signals:
	void stop();
	void log(const QString &msg);

private:
	IKinectSensor* m_kinectSensor{ nullptr };
	BodyTracker* m_bodyTracker{ nullptr };

	NetworkHandler m_networkHandler;

	QThread m_workerThread;
};

