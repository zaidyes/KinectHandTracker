#pragma once

#include <QObject>
#include <Kinect.h>
#include <iostream>

class BodyTracker :public QObject
{

	Q_OBJECT

public:
	BodyTracker(QObject *parent = 0);
	~BodyTracker();
	
	void start();

	public slots:
	void onStartRequest();

signals:
	void newLog(const QString &msg);

private:
	void processBodies(const unsigned int &bodyCount, IBody **bodies);
};

