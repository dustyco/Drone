
#include "DroneFlight.h"

#include "../Shared/Messages.h"
#include "../Shared/Discover.h"
#include "../Shared/Config.h"
#include <iostream>


DroneFlight::DroneFlight (QObject *parent) : QObject(parent)
{
	// Print the version
	QString version(GIT_VERSION);
	std::cout << "DroneFlight " << version.toStdString() << std::endl;

	// Load the configuration
	Config& config = Config::getSingleton();
	config.prefix = "Flight/";
	QString configError = checkConfig();
	if (not configError.isEmpty())
	{
		qDebug() << "Config error:" << configError;
	}
	QHostAddress towerAddress(config.value("Tower Address").toString());
	quint16 towerPort = config.value("Tower Port").toInt();

	QHostAddress multicastAddress(config.value("Multicast Address").toString());
	quint16 multicastPort = config.value("Multicast Port").toInt();

	// Start peer discovery
	Record record;
	record["Service"] = "DroneFlight";
	record["Scope"] = "Global";
	record["Channel"] = config.value("Channel").toString();

	mDiscover = new Discover(this, Discover::NormalMode, multicastAddress, multicastPort);
	connect(mDiscover, SIGNAL(recordFound(Record)), this, SLOT(recordFound(Record)));
	connect(mDiscover, SIGNAL(recordLost(Record)), this, SLOT(recordLost(Record)));
	connect(mDiscover, SIGNAL(gotDatagram(QByteArray,QHostAddress,quint16,QList<Record>)), this, SLOT(gotDatagram(QByteArray,QHostAddress,quint16,QList<Record>)));
	mDiscover->addRecord(record);
	//mDiscover->addFilter("Channel", config.value("Channel").toString());
	mDiscover->addGlobalServer(towerAddress, towerPort);
	mDiscover->start();
}

QString DroneFlight::checkConfig()
{
	Config& config = Config::getSingleton();
	QString error("Error in DroneConfig.ini [Flight]: ");

	// TODO Read the file ok

	QHostAddress towerAddress(config.value("Tower Address").toString());
	if (towerAddress.isNull()) return error + "Tower Address: " + config.value("Tower Address").toString();

	quint16 towerPort = config.value("Tower Port").toInt();
	if (towerPort < 1) return error + "Tower Port: " + config.value("Tower Port").toString();

	QHostAddress multicastAddress(config.value("Multicast Address").toString());
	if (multicastAddress.isNull()) return error + "Multicast Address: " + config.value("Multicast Address").toString();

	quint16 multicastPort = config.value("Multicast Port").toInt();
	if (multicastPort < 1) return error + "Multicast Port: " + config.value("Multicast Port").toString();

	int channel = config.value("Channel").toInt();
	if (channel < 1) return error + "Channel: " + config.value("Channel").toString();

	return "";
}

/*
	while (mSocket->hasPendingDatagrams())
	{
		// Read datagram
		QByteArray datagram;
		datagram.resize(mSocket->pendingDatagramSize());
		QHostAddress sender;
		quint16 senderPort;
		mSocket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);

		// Update ticker
		if (sender == mDiscover->mTowerAddress)
		{

		}
		else if (sender == mDiscover->mPeerAddress)
		{

		}

		// Process datagram
		QString type = "Unknown";
		if (isMessage<DiscoverResponse>(datagram))
		{
			type = "DiscoverResponse";

			// TODO Error handling
			mDiscover->receiveResponse(decodeMessage<DiscoverResponse>(datagram), sender, senderPort);
		}
		else if (isMessage<Ping>(datagram))
		{
			type = "Ping";

			// TODO Error handling
			mDiscover->receivePing(decodeMessage<Ping>(datagram), sender, senderPort);
		}
		else std::cout << "Unexpected message type" << std::endl;

		// Log the traffic
		//logTrafficIn(type, datagram.size());

	}

*/



void DroneFlight::recordFound(Record record)
{
	// TODO
	Q_UNUSED(record);
}

void DroneFlight::recordLost(Record record)
{
	// TODO
	Q_UNUSED(record);
}

void DroneFlight::gotDatagram(QByteArray datagram, QHostAddress sender, quint16 senderPort, QList<Record> matchingRecords)
{
	qDebug() << "DroneControl::gotDatagram()" << sender.toString() << senderPort;

	// TODO
	Q_UNUSED(datagram);
	Q_UNUSED(sender);
	Q_UNUSED(senderPort);
	Q_UNUSED(matchingRecords);
}
