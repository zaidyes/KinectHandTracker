#include "NetworkHandler.h"
#include "globals.h"

char* CommandStrings[static_cast<int>(Commands::END) + 1] = {
	"START",
	"BODY_TRACKED",
	"ENGAGED",
	"LEFT_HAND_TRACKED",
	"RIGHT_HAND_TRACKED",
	"LEFT_HAND_GRAB",
	"RIGHT_HAND_GRAB",
	"LEFT_HAND_OPEN",
	"RIGHT_HAND_OPEN",
	"UNKNOWN_STATE",
	"END"
};


NetworkHandler::NetworkHandler()
{
	m_udpSocket = new QUdpSocket(this);
}


NetworkHandler::~NetworkHandler()
{
}

bool NetworkHandler::connect()
{
	if (!m_udpSocket) return false;

	m_udpSocket->bind(QHostAddress::LocalHost, 4242);

	QObject::connect(m_udpSocket, &QUdpSocket::readyRead, this, &NetworkHandler::read);
	return true;
}

void NetworkHandler::onCommand(const int command)
{
	if (command > static_cast<int>(Commands::END)) return;

	write(CommandStrings[command]);
}

void NetworkHandler::read()
{
	while (m_udpSocket->hasPendingDatagrams()) {
		QNetworkDatagram datagram = m_udpSocket->receiveDatagram();
		emit incomingData(QString(datagram.data()));
	}
}

void NetworkHandler::write(const QByteArray & data)
{
	m_udpSocket->writeDatagram(data, QHostAddress::LocalHost, 4242);
}

void NetworkHandler::onMessage(const QString& msg)
{
	if (!m_udpSocket) return;

	write(msg.toLocal8Bit());
}
