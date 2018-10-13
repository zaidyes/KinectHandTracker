#pragma once

#include <QObject>

#include <ITracker.h>

enum class Commands;

struct BodyData {
	enum class State {
		ENGAGED,
		TRACKED,
		OPEN,
		CLOSE,
		UNKNOWN
	};

	BodyData(ulong id) :
		leftHand{State::UNKNOWN},
		rightHand{State::UNKNOWN},
		attention{State::UNKNOWN},
		body{State::UNKNOWN},
		id{id} {};

	inline bool operator==(const BodyData& a) {
		return (leftHand == a.leftHand && rightHand == a.rightHand && attention == a.attention && body == a.body && id == a.id);
	}

	State leftHand;
	State rightHand;
	State attention;
	State body;
	ulong id;
};

class BodyTracker : public ITracker
{

	Q_OBJECT
public:
	BodyTracker(QObject *parent = 0);
	~BodyTracker();

protected:
	void start() final;

signals:
	void command(const Commands cmd);

private:
	void processBodies(const unsigned int &bodyCount, IBody **bodies);
	void bodyChanged(const BodyData & newBody, const BodyData & oldBody);
	std::vector<BodyData> m_bodies;
};

