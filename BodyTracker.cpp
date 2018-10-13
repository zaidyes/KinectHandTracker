#include "BodyTracker.h"

#include <globals.h>

BodyTracker::BodyTracker(QObject *parent)
	: ITracker(parent)
{
}


BodyTracker::~BodyTracker()
{
	m_bodies.clear();
}

void BodyTracker::start()
{
	if (!m_sensor) {
		emit log("ERROR:Cannot find any sensors.");
		return;
	}

	m_isRunning = true;
	emit log("DEBUG:Body tracker started");

	//Get a body frame source from which we can get our body frame reader
	IBodyFrameSource *bodyFrameSource = nullptr;
	IBodyFrameReader *bodyFrameReader = nullptr;

	HRESULT hr = m_sensor->get_BodyFrameSource(&bodyFrameSource);

	if (SUCCEEDED(hr)) {
		hr = bodyFrameSource->OpenReader(&bodyFrameReader);
	}

	//We're done with bodyFrameSource, so we'll release it
	safeRelease(bodyFrameSource);
	emit log("INFO:Starting body tracking");

	while (bodyFrameReader != nullptr) {

		if (m_stop) {
			emit log("INFO:Stopping");
			return;
		}

		IBodyFrame *bodyFrame = nullptr;
		hr = bodyFrameReader->AcquireLatestFrame(&bodyFrame);

		if (SUCCEEDED(hr)) {
			IBody *bodies[BODY_COUNT] = { 0 };
			hr = bodyFrame->GetAndRefreshBodyData(_countof(bodies), bodies);
			
			if (SUCCEEDED(hr)) {
				processBodies(BODY_COUNT, bodies);
				//After body processing is done, we're done with our bodies so release them.
				for (unsigned int bodyIndex = 0; bodyIndex < _countof(bodies); bodyIndex++) {
					safeRelease(bodies[bodyIndex]);
				}

				safeRelease(bodyFrame);
			}
		}
	}

	m_isRunning = false;
}

void BodyTracker::processBodies(const unsigned int &bodyCount, IBody **bodies)
{
	for (unsigned int bodyIndex = 0; bodyIndex < bodyCount; bodyIndex++) {
		IBody *kBody = bodies[bodyIndex];

		//Get the tracking status for the body, if it's not tracked we'll skip it
		BOOLEAN isTracked = false;
		HRESULT hr = kBody->get_IsTracked(&isTracked);
		if (FAILED(hr) || isTracked == false) {
			continue;
		}

		// create our body
		UINT64 trackingId;
		hr = kBody->get_TrackingId(&trackingId);

		if (FAILED(hr)) continue;

		BodyData newBodyData(trackingId);
		emit log("Body Tracked");

		DetectionResult detectionRes;
		hr = kBody->get_Engaged(&detectionRes);
		if (SUCCEEDED(hr)) {
			if (detectionRes == DetectionResult_Maybe || detectionRes == DetectionResult_Yes) {
				newBodyData.attention = BodyData::State::ENGAGED;
				emit log("Body engaged");
			}
		}


		//If we're here the body is tracked so lets get the joint properties for this skeleton
		Joint joints[JointType_Count];
		hr = kBody->GetJoints(_countof(joints), joints);
		if (SUCCEEDED(hr)) {
			//Let's print the head's position
			const CameraSpacePoint &headPos = joints[JointType_Head].Position;
			const CameraSpacePoint &leftHandPos = joints[JointType_HandLeft].Position;
			const CameraSpacePoint &rightHandPos = joints[JointType_HandRight].Position;

			HandState leftHandState;
			hr = kBody->get_HandLeftState(&leftHandState);
			if (SUCCEEDED(hr)) {
				if (leftHandState != HandState_NotTracked || leftHandState != HandState_Unknown) {
					newBodyData.leftHand = BodyData::State::UNKNOWN;
					emit log("left hand tracked");
				}
				if (leftHandState == HandState_Closed) {
					newBodyData.leftHand = BodyData::State::CLOSE;
					emit log("left hand grab");
				}
				if (leftHandState == HandState_Open) {
					newBodyData.leftHand = BodyData::State::OPEN;
					emit log("left hand open");
				}
			}

			HandState rightHandState;
			hr = kBody->get_HandRightState(&rightHandState);
			if (SUCCEEDED(hr)) {
				if (rightHandState != HandState_NotTracked || rightHandState != HandState_Unknown) {
					newBodyData.rightHand = BodyData::State::UNKNOWN;
					emit log("right hand tracked");
				}
				if (rightHandState == HandState_Closed) {
					newBodyData.rightHand = BodyData::State::CLOSE;
					emit log("right hand grab");
				}
				if (rightHandState == HandState_Open) {
					newBodyData.rightHand = BodyData::State::OPEN;
					emit log("right hand open");
				}
			}
			
			auto body = std::find_if(m_bodies.begin(), m_bodies.end(), [&newBodyData](const BodyData & m) -> bool { return m.id == newBodyData.id; });
			if (body != m_bodies.end()) {
				if (*body == newBodyData) continue;

				bodyChanged(newBodyData, *body);
				*body = newBodyData;
			} else {
				m_bodies.push_back(newBodyData);
			}
			

			//Let's check if the use has his hand up

			/*if (leftHandPos.Y >= headPos.Y) {
				emit log("INFO:LEFT HAND UP!!");
			}

			HandState leftHandState;
			hr = body->get_HandLeftState(&leftHandState);
			if (SUCCEEDED(hr)) {
				if (leftHandState == HandState_Closed) {
					emit log("INFO:CLOSED HAND");
				}
				else if (leftHandState == HandState_Open) {
					emit log("INFO:OPEN HAND");
				}
				else if (leftHandState == HandState_Lasso) {
					emit log("INFO:PEW PEW HANDS");
				}
				else if (leftHandState == HandState_NotTracked) {
					emit log("INFO:HAND IS NOT TRACKED");
				}
				else if (leftHandState == HandState_Unknown) {
					emit log("INFO:HANDS STATE IS UNKNOWN");
				}
			}*/
		}
	}
}

void BodyTracker::bodyChanged(const BodyData & newBody, const BodyData & oldBody)
{
	emit command(Commands::START);
	
	if (newBody.attention != oldBody.attention) {
		emit command(Commands::ATTENTION);
		emit command((newBody.attention == BodyData::State::ENGAGED) ? Commands::ENGAGED : Commands::UNKNOWN_STATE);
	}
	
	if (newBody.body != oldBody.body) {
		emit command(Commands::BODY);
		if (newBody.body == BodyData::State::TRACKED)
			emit command(Commands::TRACKED);
		if (newBody.body == BodyData::State::UNKNOWN)
			emit command(Commands::UNKNOWN_STATE);
	}

	if (newBody.leftHand != oldBody.leftHand) {
		emit command(Commands::LEFT_HAND);
		if (newBody.leftHand == BodyData::State::TRACKED)
			emit command(Commands::TRACKED);
		else if (newBody.leftHand == BodyData::State::OPEN)
			emit command(Commands::OPEN);
		else if (newBody.leftHand == BodyData::State::CLOSE)
			emit command(Commands::CLOSE);
		else if (newBody.leftHand == BodyData::State::UNKNOWN)
			emit command(Commands::UNKNOWN_STATE);
	}

	if (newBody.rightHand != oldBody.rightHand) {
		emit command(Commands::RIGHT_HAND);
		if (newBody.rightHand == BodyData::State::TRACKED)
			emit command(Commands::TRACKED);
		else if (newBody.rightHand == BodyData::State::OPEN)
			emit command(Commands::OPEN);
		else if (newBody.rightHand == BodyData::State::CLOSE)
			emit command(Commands::CLOSE);
		else if (newBody.rightHand == BodyData::State::UNKNOWN)
			emit command(Commands::UNKNOWN_STATE);
	}

	emit command(Commands::END);
}

