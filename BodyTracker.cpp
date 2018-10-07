//#include "moc_BodyTracker.cpp"
#include "BodyTracker.h"
#include <QtConcurrent\qtconcurrentrun.h>
#include <QApplication>


BodyTracker::BodyTracker(QObject *parent)
	: QObject(parent)
{
}


BodyTracker::~BodyTracker()
{
}

template<class Interface>
static inline void safeRelease(Interface *&interfaceToRelease)
{
	if (interfaceToRelease != nullptr) {
		interfaceToRelease->Release();
		interfaceToRelease = nullptr;
	}
}

void BodyTracker::start()
{
	IKinectSensor *sensor = nullptr;
	IBodyFrameReader *bodyFrameReader = nullptr;

	//Get the default Kinect sensor
	HRESULT hr = GetDefaultKinectSensor(&sensor);

	//If the function succeeds, open the sensor
	if (SUCCEEDED(hr)) {
		hr = sensor->Open();

		if (SUCCEEDED(hr)) {
			//Get a body frame source from which we can get our body frame reader
			IBodyFrameSource *bodyFrameSource = nullptr;
			hr = sensor->get_BodyFrameSource(&bodyFrameSource);

			if (SUCCEEDED(hr)) {
				hr = bodyFrameSource->OpenReader(&bodyFrameReader);
			}

			//We're done with bodyFrameSource, so we'll release it
			safeRelease(bodyFrameSource);
		}
	}

	if (sensor == nullptr || FAILED(hr)) {
		emit newLog("ERROR:Cannot find any sensors.");
	}

	while (bodyFrameReader != nullptr) {
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
		else if (sensor) {
			BOOLEAN isSensorAvailable = false;
			hr = sensor->get_IsAvailable(&isSensorAvailable);
			if (SUCCEEDED(hr) && isSensorAvailable == false) {
				emit newLog("ERROR:No available sensor is found.");
			}
		}
		else {
			emit newLog("ERROR:Trouble reading the body frame.");
		}
		QApplication::processEvents();
	}
}

void BodyTracker::processBodies(const unsigned int &bodyCount, IBody **bodies)
{
	for (unsigned int bodyIndex = 0; bodyIndex < bodyCount; bodyIndex++) {
		IBody *body = bodies[bodyIndex];

		//Get the tracking status for the body, if it's not tracked we'll skip it
		BOOLEAN isTracked = false;
		HRESULT hr = body->get_IsTracked(&isTracked);
		if (FAILED(hr) || isTracked == false) {
			continue;
		}

		//If we're here the body is tracked so lets get the joint properties for this skeleton
		Joint joints[JointType_Count];
		hr = body->GetJoints(_countof(joints), joints);
		if (SUCCEEDED(hr)) {
			//Let's print the head's position
			const CameraSpacePoint &headPos = joints[JointType_Head].Position;
			const CameraSpacePoint &leftHandPos = joints[JointType_HandLeft].Position;

			//Let's check if the use has his hand up
			if (leftHandPos.Y >= headPos.Y) {
				emit newLog("INFO:LEFT HAND UP!!");
			}

			HandState leftHandState;
			hr = body->get_HandLeftState(&leftHandState);
			if (SUCCEEDED(hr)) {
				if (leftHandState == HandState_Closed) {
					emit newLog("INFO:CLOSED HAND");
				}
				else if (leftHandState == HandState_Open) {
					emit newLog("INFO:OPEN HAND");
				}
				else if (leftHandState == HandState_Lasso) {
					emit newLog("INFO:PEW PEW HANDS");
				}
				else if (leftHandState == HandState_NotTracked) {
					emit newLog("INFO:HAND IS NOT TRACKED");
				}
				else if (leftHandState == HandState_Unknown) {
					emit newLog("INFO:HANDS STATE IS UNKNOWN");
				}
			}
		}
	}
}

void BodyTracker::onStartRequest()
{
	start();
	//QtConcurrent::run(this, &BodyTracker::start);
}
