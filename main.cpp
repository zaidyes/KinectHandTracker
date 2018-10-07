#include "qtkinect.h"
#include <QtWidgets/QApplication>
#include <QObject>

#include <BodyTracker.h>


int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	BodyTracker bodyTracker;
	qtkinect w;

	QObject::connect(&bodyTracker, &BodyTracker::newLog, &w, &qtkinect::addLog);
	QObject::connect(&w, &qtkinect::startTracking, &bodyTracker, &BodyTracker::onStartRequest);

	w.show();
	return a.exec();
}
