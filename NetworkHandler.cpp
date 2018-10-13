#include "NetworkHandler.h"
#include "globals.h"

char* CommandStrings[static_cast<int>(Commands::END) + 1] = {
	"START",
	"BODY",
	"ATTENTION",
	"TRACKED",
	"ENGAGED",
	"LEFT_HAND",
	"RIGHT_HAND",
	"CLOSE",
	"OPEN",
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
	if (!m_udpSocket || !m_config.activate) return false;

	m_udpSocket->bind(QHostAddress(m_config.ipAddress), m_config.port);

	QObject::connect(m_udpSocket, &QUdpSocket::readyRead, this, &NetworkHandler::read);
	return true;
}

void NetworkHandler::setConfig(const NetworkConfig & networkConfig)
{
	m_config = networkConfig;
}

void NetworkHandler::onCommand(const Commands command)
{
	if (command > Commands::END) return;

	write(CommandStrings[static_cast<int>(command)]);
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
	if (!m_udpSocket || !m_config.activate) return;

	emit log("DEBUG: Writing " + data + " to: " + m_config.ipAddress + "/" + QString::number(m_config.port));
	m_udpSocket->writeDatagram(data, QHostAddress(m_config.ipAddress), m_config.port);
}

void NetworkHandler::onMessage(const QString& msg)
{
	write(msg.toLocal8Bit());
}
