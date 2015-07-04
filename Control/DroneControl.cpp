

#include "DroneControl.h"
#include "../Shared/Messages.h"
#include <QCoreApplication>

DroneControl::DroneControl(QObject* parent) : QObject(parent)
{
	// Print the version
	QString version(GIT_VERSION);
	qDebug() << "DroneControl" << version;

	// Load the configuration
	Config& config = Config::getSingleton();
	config.prefix = "Control/";
	QString configError = checkConfig();
	if (not configError.isEmpty())
	{
		qDebug() << "Config error:" << configError;
	}
	QHostAddress towerAddress(config.value("Tower Address").toString());
	quint16 towerPort = config.value("Tower Port").toInt();

	QHostAddress multicastAddress(config.value("Multicast Address").toString());
	quint16 multicastPort = config.value("Multicast Port").toInt();

	// Cache a copy of the flight filter for speed and ease
	// So it doesn't need to be created for every outgoing message
	mFlightFilter["Service"] = "DroneFlight";
	mFlightFilter["Channel"] = config.value("Channel").toString();

	// Set up pinging
	mPingCounter = 0;
	mPingTimer = new QTimer(this);
	connect(mPingTimer, SIGNAL(timeout()), this, SLOT(sendPing()));

	// Start peer discovery
	Record record;
	record["Service"] = "DroneControl";
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

QString DroneControl::info() const {
	QString _info;

	// Print the Flight descriptions
	_info += "Flight:\n";
	for (QString description : mFlightDescriptions)
	{
		_info += "  ";
		_info += description;
		_info += "\n";
	}

	return _info;
}

qreal DroneControl::frameTrigger() const { return 0; }

void DroneControl::setFrameTrigger(qreal frameTrigger)
{
	Q_UNUSED(frameTrigger);
	// TODO Check joystick input and send a packet if
	//   Joystick has moved
	//   Heartbeat interval has passed
	//   Any addresses have changed (maybe make these properties instead)
}

QString DroneControl::checkConfig()
{
	Config& config = Config::getSingleton();
	QString error("Error in DroneConfig.ini [Control]: ");

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

void DroneControl::sendPing()
{
	Ping ping;
	ping.id = mPingCounter++;
	ping.trips = 1;
	mDiscover->sendDatagramTo(encodeMessage<Ping>(ping), mFlightFilter);
}

void DroneControl::recordFound(Record record)
{
	Config& config = Config::getSingleton();
	if (record["Service"]=="DroneFlight" and record["Channel"]==config.value("Channel"))
	{
		QString description = record["Address"] + ":" + record["Port"];
		qDebug() << "Found DroneFlight:" << description;
		mFlightDescriptions.insert(description);

		// Ping
		mPingTimer->start(500);

		emit infoChanged(info());
	}
}

void DroneControl::recordLost(Record record)
{
	Config& config = Config::getSingleton();
	QString description = record["Address"] + ":" + record["Port"];
	qDebug() << "Lost DroneFlight:" << description;
	mFlightDescriptions.remove(description);
	emit infoChanged(info());
}

void DroneControl::gotDatagram(QByteArray datagram, QHostAddress sender, quint16 senderPort, QList<Record> matchingRecords)
{
	Q_UNUSED(matchingRecords);

	if (isMessage<Ping>(datagram))
	{
		Ping ping = decodeMessage<Ping>(datagram);
		if (ping.trips > 0 and ping.trips < 3)
		{
			++ping.trips;
			qDebug() << "Responding to ping #" << ping.id << "from" << sender.toString() << senderPort;
			mDiscover->sendDatagramTo(encodeMessage<Ping>(ping), sender, senderPort);
		}
	}
	else qDebug() << "DroneFlight::gotDatagram() of size" << datagram.size() << "from" << sender.toString() << senderPort;
}

/*
void DroneControl::readDatagrams()
{
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
		logTrafficIn(type, datagram.size());

	}
}
	*/
