#include "qtkinect.h"
#include <qplaintextedit.h>

qtkinect::qtkinect(QWidget *parent)
	: QMainWindow(parent)
{
	m_ui.setupUi(this);
}


void qtkinect::addLog(const QString &msg)
{
	QPlainTextEdit *logTextEdit = m_ui.pte_logWindow;
	logTextEdit->moveCursor(QTextCursor::End);
	logTextEdit->insertPlainText(msg);
	logTextEdit->moveCursor(QTextCursor::End);
}

void qtkinect::on_pb_startTracking_clicked()
{
	emit startTracking();
}
