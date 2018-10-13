#pragma once

#include <QObject>
#include <QtNetwork>

enum class Commands;

struct NetworkConfig
{
	NetworkConfig() : ipAddress{ "" }, port{ 0 }, activate{ false } {};

	QString ipAddress;
	ulong port;
	bool activate;
};

class NetworkHandler : public QObject
{
	Q_OBJECT

public:
	NetworkHandler();
	~NetworkHandler();

	bool connect();

	void setConfig(const NetworkConfig &networkConfig);

signals:
	void incomingData(const QString &data);
	void log(const QString &msg);

public slots:
	void onMessage(const QString &msg);
	void onCommand(const Commands command);
	void read();

protected:
	void write(const QByteArray &data);

private:
	QUdpSocket* m_udpSocket;
	NetworkConfig m_config;
};

