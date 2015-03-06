


#include "Discover.h"
#include <QtNetwork>
#include <QDateTime>
#include "Identity.h"



Discover::Discover(QObject *parent) : QObject(parent)
{
	port = 45454;
	groupAddress = QHostAddress("239.255.43.21");
	announcePeriodMsec = 500;
	running = false;
	mServerMode = false;
	announceNeedsResponse = true;
	departure = false;
	
	// Timers
	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(announceRecords()));
	expireTimer = new QTimer(this);
	connect(expireTimer, SIGNAL(timeout()), this, SLOT(expireRecords()));
	
	// Global socket
	loopbackSocket = new QUdpSocket(this);
	connect(loopbackSocket, SIGNAL(readyRead()), this, SLOT(readDatagrams()));
	if (not loopbackSocket->bind(QHostAddress::AnyIPv4, port, QAbstractSocket::ShareAddress | QUdpSocket::ReuseAddressHint))
	{
		qDebug() << "Error binding loopbackSocket:" << loopbackSocket->errorString();
	}

}

Discover::~Discover()
{
	//qDebug() << "Deleting";
	departure = true;
	announceRecords();
	running = false;
}

void Discover::addGlobalServer (QString globalServer)
{
	// TODO
	Q_UNUSED(globalServer);
	
	// Announce it immediately after returning to event loop
	if (running and not mServerMode) timer->start(0);
}

void Discover::addFilter (QString key, QString value)
{
	filters[key] = value;
}

void Discover::addRecord (Record record)
{
	// Inject a default Scope
	if (record["Scope"].isEmpty()) record["Scope"] = "Local";
	record["Instance"] = Identity::basedOnInterfaces(QByteArray::number(quint64(this)));
	record["Machine"] = Identity::basedOnInterfaces();

	// These will be announced or given in responses
	// TODO Instance UUID injection and maybe other reserved values
	ownedRecords.push_back(record);
	
	qDebug() << "Owned record:" << record.toString();
	
	// Announce it immediately after returning to event loop
	if (running and not mServerMode) timer->start(0);
}

void Discover::setGlobalServerMode (bool b)
{
	mServerMode = b;
	
	if (mServerMode)
	{
		// Server's don't announce periodically
		timer->stop();
	}
	else
	{
		// Announce it immediately after returning to event loop
		if (running) timer->start(0);
	}
}

bool Discover::start ()
{
	// TODO Return false if there's an error
	
	// Announce it immediately after returning to event loop
	if (not mServerMode) timer->start(0);
	running = true;
	
	// TODO Errors? Maybe send an error signal instead
	return true;
}

void Discover::readDatagrams()
{
	QUdpSocket* socket = qobject_cast<QUdpSocket*>(QObject::sender());
	if (socket)
	while (socket->hasPendingDatagrams())
	{
		// Read it to a buffer
		QByteArray datagram;
		datagram.resize(socket->pendingDatagramSize());
		QHostAddress sender;
		quint16 senderPort;
		socket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);

		// Check the prefix
		bool respond = false;
		bool removeThem = false;
			 if (datagram.startsWith("DSDA")) respond = false; // Announcement, do not respond
		else if (datagram.startsWith("DSDR")) respond = true; // Request, do respond
		else if (datagram.startsWith("DSDD")) removeThem = true; // This is a departure, remove records following
		else continue; // Ignore it
		datagram.remove(0, 4);

		// Determine the scope of the sender
		// See if it's this machine
		QString scope("Global");
		if (sender.isLoopback())
		{
			scope = "Loopback";
		}
		else if (sender.protocol() == QAbstractSocket::IPv4Protocol)
		{
			quint32 senderInt = sender.toIPv4Address();
			for (auto iface : QNetworkInterface::allInterfaces())
			for (auto entry : iface.addressEntries())
			if (entry.ip().protocol() == QAbstractSocket::IPv4Protocol)
			{
				quint32 netmaskInt = entry.netmask().toIPv4Address();
				quint32 entryInt = entry.ip().toIPv4Address();
				if ((senderInt & netmaskInt) == (entryInt & netmaskInt))
				{
					// Came from the local network
					scope = "Local";
				}
			}
		}

		//
		acceptRecords(Record::listFromBytes(datagram), removeThem, scope, sender.toString(), QString::number(senderPort));

		// Normal: Respond to only Local requests with only owned Records
		if (respond and not mServerMode and (scope=="Local" or scope=="Loopback"))
		{
			socket->writeDatagram(QByteArray("DSDA")+Record::listToBytes(ownedRecords), sender, senderPort);
		}
		// Server: Respond to only Global requests with owned and found Records
		else if (respond and mServerMode)
		{
			socket->writeDatagram(QByteArray("DSDA")+Record::listToBytes(ownedRecords + foundRecords.keys()), sender, senderPort);
		}
	}
}

void Discover::announceRecords ()
{
	// Reset timer
	timer->start(announcePeriodMsec);

	if (not running) return;

	// Servers don't announce periodically or send departure messages
	if (mServerMode) return;

	// Start building a list of records to send in each scope
	QList<Record> recordsToSend;

	///////////////////////////////////////////////////////////////////////
	// Global scope first
	///////////////////////////////////////////////////////////////////////
	for (Record record : ownedRecords)
	if (record["Scope"] == "Global")
	{
		record.remove("Scope");
		recordsToSend.push_back(record);
	}
	if (not recordsToSend.empty())
	{
		// This is a discovery request which should be responded to
		QByteArray datagram("DSDR");
		if (departure) datagram = "DSDD";
		datagram += Record::listToBytes(recordsToSend);

		// TODO Send to each Global server
		qDebug() << "Sending to global server";
	}

	///////////////////////////////////////////////////////////////////////
	// Local scope next
	///////////////////////////////////////////////////////////////////////
	for (Record record : ownedRecords)
	if (record["Scope"] == "Local")
	{
		record.remove("Scope");
		recordsToSend.push_back(record);
	}
	if (not recordsToSend.empty())
	{
		QByteArray datagram("DSDA");
		if (announceNeedsResponse) datagram = "DSDR";
		if (departure)             datagram = "DSDD";
		datagram += Record::listToBytes(recordsToSend);

		// For each interface
		for (auto iface : QNetworkInterface::allInterfaces())
		{
			// For each IPv4 address on this interface
			for (auto entry : iface.addressEntries())
			{
				if (entry.ip().protocol() != QAbstractSocket::IPv4Protocol) continue;

				// TODO Multicast
				if (iface.flags().testFlag(QNetworkInterface::CanMulticast) and !entry.ip().isNull())
				{
					// Create the socket if it doesn't exit yet
					if (not multiSocket.contains(entry.ip().toString()))
					{
						qDebug() << "New multicast socket: " << entry.ip().toString();
						// Add it, create and bind the socket
						QUdpSocket* socket = new QUdpSocket(this);
						connect(socket, SIGNAL(readyRead()), this, SLOT(readDatagrams()));
						socket->setSocketOption(QAbstractSocket::MulticastTtlOption, 1);
						if (not socket->bind(QHostAddress::AnyIPv4, port, QAbstractSocket::ShareAddress | QAbstractSocket::ReuseAddressHint))
						{
							qDebug() << "Error binding to iface" << entry.ip().toString() << socket->errorString();
						}
						socket->setMulticastInterface(iface);
						socket->joinMulticastGroup(groupAddress, iface);
						multiSocket.insert(entry.ip().toString(), socket);
					}

					// Send datagram
					multiSocket[entry.ip().toString()]->writeDatagram(datagram, groupAddress, port);
				}
			}
		}
	}

	///////////////////////////////////////////////////////////////////////
	// Local scope last
	///////////////////////////////////////////////////////////////////////
	for (Record record : ownedRecords)
	if (record["Scope"] == "Loopback")
	{
		record.remove("Scope"); // Scope has a different meaning on the receiving end
		recordsToSend.push_back(record);
	}
	if (not recordsToSend.empty())
	{
		QByteArray datagram("DSDA");
		if (announceNeedsResponse) datagram = "DSDR";
		if (departure)             datagram = "DSDD";
		datagram += Record::listToBytes(recordsToSend);

		// Loopback
		loopbackSocket->writeDatagram(datagram, QHostAddress::LocalHost, port);
	}

	// Reset
	announceNeedsResponse = false;
	departure = false;
}

void Discover::expireRecords ()
{
	int next = 1000; // Maximum default interval for expiration
	quint64 now = QDateTime::currentMSecsSinceEpoch();
	for (const auto& pair : foundRecords.toStdMap())
	{
		int delta = pair.second - now;
		if (delta <= 0)
		{
			// It's expired
			qDebug() << "Lost Record:" << pair.first.toString();
			foundRecords.remove(pair.first);
			emit recordLost(pair.first);
		}
		else if (delta < next) next = delta; // Set the timer for this one
	}
	// Set timer for next time
	expireTimer->start(next);
}

void Discover::acceptRecords (QList<Record> records, bool removeThem, QString senderScope, QString senderAddress, QString senderPort)
{
	// TODO
	
	for (Record& record : records)
	{
		// Preprocess the record before considering it
		Record::decompressReserved(record);
		record["Scope"] = senderScope; // No reason for the sender to ever include this
		if (record["Address"].isEmpty()) record["Address"] = senderAddress; // Sender may override these
		if (record["Port"].isEmpty()) record["Port"] = senderPort; // Sender may override these
		// TODO Extract a custom expiration duration so each record can have its own


		if (removeThem)
		{
			// See if it's new
			if (foundRecords.contains(record))
			{
				qDebug() << "Record Departed:" << record.toString();
				foundRecords.remove(record);
				emit recordLost(record);
			}
		}
		else
		{
			// See if it's new
			if (not foundRecords.contains(record) and passesFilters(record))
			{
				qDebug() << "Found Record:" << record.toString();
				emit recordFound(record);
			}

			// Reset the timer
			foundRecords[record] = QDateTime::currentMSecsSinceEpoch() + 2500; // Set the expire time
		}

		expireTimer->start(0); // In case the next expiration time was changed
	}
}

bool Discover::passesFilters (Record record)
{
	// No filters, default pass
	if (filters.isEmpty()) return true;
	
	// Try to eliminate
	for (QString key : filters.keys())
	{
		if (not record.has(key) or record[key] != filters[key]) return false;
	}
	
	// It's good
	return true;
}




