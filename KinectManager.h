#pragma once

#include <QObject>
#include <QThread>

#include <BodyTracker.h>

class KinectManager : public QObject
{
	Q_OBJECT

public:
	KinectManager(QObject *parent = 0);
	~KinectManager();

	void startKinect();
	void stopKinect();

signals:
	void stop();
	void onMessage(const QString &msg);

private:
	BodyTracker* m_bodyTracker{ nullptr };
	QThread m_workerThread;
};

