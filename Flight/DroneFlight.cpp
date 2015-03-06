
#include "DroneFlight.h"

#include "../Shared/Messages.h"
#include "../Shared/Discover.h"
#include "../Shared/Config.h"



DroneFlight::DroneFlight (QObject *parent) : QObject(parent)
{
	// Load the configuration
	Config& config = Config::getSingleton();
	config.prefix = "Flight/";
	//quint16 port = config.value("Port").toInt();



	// Set up socket
/*	mSocket = new QUdpSocket(this);
	connect(mSocket, SIGNAL(readyRead()), this, SLOT(readDatagrams()));
	if (not mSocket->bind(QHostAddress::Any, port))
		std::cout << "Failed to bind to port " << port << std::endl;
	else std::cout << "Using port " << port << std::endl;
*/
	//mTraffic = 0;

	// Start peer discovery (may send first signal immediately, resolve deps first
/*        mDiscover = new Discover(this);
	connect(mDiscover, &Discover::sendDatagram, this, &DroneFlight::sendDatagram);
	connect(mDiscover, &Discover::towerChanged, this, &DroneFlight::towerChanged);
	connect(mDiscover, &Discover::peerChanged, this, &DroneFlight::peerChanged);
	mDiscover->start("Flight");
*/

	// Start peer discovery
	Record record;
	record["Service"] = "DroneFlight";
	record["Scope"] = "Local";

	mDiscover = new Discover(this);
	//connect(mDiscover, SIGNAL(recordFound(Record)), this, SLOT(recordFound(Record)));
	//connect(mDiscover, SIGNAL(recordLost(Record)), this, SLOT(recordLost(Record)));
	mDiscover->addRecord(record);
	mDiscover->start();
}
void DroneFlight::readDatagrams ()
{/*
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

*/}

void DroneFlight::towerChanged (QHostAddress address, quint16 port)
{
	std::cout << "Tower address changed (" << address.toString().toStdString() << ":" << port << ")" << std::endl;
	//doNetworkStatus();
}
void DroneFlight::peerChanged (QHostAddress address, quint16 port)
{
	std::cout << "Peer address changed (" << address.toString().toStdString() << ":" << port << ")" << std::endl;
	//doNetworkStatus();
}

void DroneFlight::sendDatagram (QByteArray datagram, QString messageType, QHostAddress address, quint16 port)
{
	Q_UNUSED(messageType);
	// TODO
	//logTrafficOut(messageType, datagram.size());
	mSocket->writeDatagram(datagram, address, port);
}
