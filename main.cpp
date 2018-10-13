#include "qtkinect.h"
#include <QtWidgets/QApplication>
#include <QObject>

#include <BodyTracker.h>


int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	qtkinect w;

	w.show();
	return a.exec();
}
