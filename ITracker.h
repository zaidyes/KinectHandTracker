#pragma once

//std includes
#include <atomic>

//Qt includes
#include <QObject>

// Kinect includes
#include <Kinect.h>

class ITracker : public QObject
{

	Q_OBJECT

public:
	ITracker(QObject *parent = 0);
	virtual ~ITracker();

	bool isRunning() const {
		return m_isRunning;
	}

	bool isValid() const {
		return (m_sensor != 0);
	}

	void setSensor(IKinectSensor* sensor) { 
		m_sensor = sensor; 
	}

public slots:
	void onStartRequest() { 
		if (!m_isRunning)
			start(); 
	}

	void onStopRequest() { 
		m_stop = true;
	}

signals:
	void log(const QString &msg);

protected:
	virtual void start() = 0;


	IKinectSensor* m_sensor{ nullptr };

	//flags
	std::atomic_bool m_stop{ false };
	std::atomic_bool m_isRunning{ false };
};

