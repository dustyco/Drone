


#include "Discover.h"
#include <QtNetwork>
#include <QDateTime>
#include "Identity.h"


// TODO
// Server: Forwards new global records to all global peers
// Both modes: Broadcasts departure directly to global peers (test without global server forwarding)

Discover::Discover(QObject *parent, Mode mode, QHostAddress multicastAddress, quint16 multicastPort) : QObject(parent)
{
	mServerMode = (mode == ServerMode);
	port = multicastPort;
	groupAddress = multicastAddress;
	announcePeriodMsec = 500;
	running = false;
	announceNeedsResponse = true;
	departure = false;
	defaultScope = "Local";

	setLoggerTag("Discover");

	// Timers
	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(announceRecords()));
	expireTimer = new QTimer(this);
	connect(expireTimer, SIGNAL(timeout()), this, SLOT(expireRecords()));

	// Server mode?
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

	// Loopback socket
//	loopbackSocket = new QUdpSocket(this);
//	connect(loopbackSocket, SIGNAL(readyRead()), this, SLOT(readDatagrams()));
//	if (not loopbackSocket->bind(QHostAddress::LocalHost, port, QAbstractSocket::ShareAddress | QAbstractSocket::ReuseAddressHint))
//	{
//		qlogDebug() << "Error binding loopbackSocket:" << loopbackSocket->errorString();
//	}

	// Global socket
	globalSocket = new QUdpSocket(this);
	connect(globalSocket, SIGNAL(readyRead()), this, SLOT(readDatagrams()));
	if (mServerMode) logDebug(QString("Discover global server mode on port %1").arg(port));
	if (not globalSocket->bind(QHostAddress::AnyIPv4, mServerMode?port:0))
	{
		logWarning(QString("Error binding globalSocket: %1").arg(globalSocket->errorString()));
	}

}

Discover::~Discover()
{
	//logDebug("Running destructor");
	departure = true;
	announceRecords();
	running = false;
}

void Discover::addGlobalServerParse (QString globalServer)
{
	// TODO DNS name resolution
	// TODO Port parsing and default definition
	addGlobalServer(QHostAddress(globalServer), 45454);
}

void Discover::addGlobalServer (QHostAddress address, quint16 port)
{
	if (address.isNull()) logWarning(QString("addGlobalServer(): Bad server address: %1").arg(address.toString()));
	else globalServers.push_back(QPair<QHostAddress,quint16>(address, port));

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
	ownedRecords.push_back(record);
	
	logDebug(QString("Owned record: %1").arg(record.toString()));
	
	// Announce it immediately after returning to event loop
	if (running and not mServerMode) timer->start(0);
}


void Discover::setDefaultScope (QString scope)
{
	defaultScope = scope;

	// Announce immediately after returning to event loop
	if (running and not mServerMode) timer->start(0);
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

bool Discover::sendDatagramTo (QByteArray datagram, Record filter, Discover::SendMode sendMode)
{
	Q_UNUSED(sendMode); // TODO
	//logDebug(QString("Sending datagram of size %1 to %2").arg(datagram.size()).arg(filter.toString()));

	QList<Record> matchingRecords;

	// Filter found records that only match this one
	for (Record record : foundRecords.keys())
	{
		bool useRecord = true;
		for (const QString& key : filter.keys())
		{
			if (!record.has(key) or record[key] != filter[key])
			{
				useRecord = false;
				break;
			}
		}
		if (useRecord) matchingRecords.append(record);
	}
	if (matchingRecords.empty()) return false;

	// Send to these records
	// TODO Use SendMode
	for (Record record : matchingRecords)
	{
		// Get address
		QHostAddress address(record["Address"]);
		bool ok = false;
		quint16 port = record["Port"].toInt(&ok);

		//logDebug(QString("    Route: %1:%2").arg(address.toString()).arg(port));
		sendDatagramTo(datagram, address, port);
	}
	return true;
}

bool Discover::sendDatagramTo(QByteArray datagram, QHostAddress address, quint16 port)
{
	if (false and addressIsLocal(address))
	{
		// Iterate multicast sockets
		for (QUdpSocket* socket : multiSocket.values())
			socket->writeDatagram(datagram, address, port);
	}
	else globalSocket->writeDatagram(datagram, address, port);
}

void Discover::readDatagrams ()
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
		else
		{
			// This datagram isn't written for Discover, pass it on
			QList<Record> matchingRecords;
			for (Record record : foundRecords.keys())
			{
				if (record.has("Address") and record["Address"]==sender.toString())
				if (record.has("Port") and record["Port"]==QString::number(senderPort))
					matchingRecords.append(record);
			}
			emit gotDatagram(datagram, sender, senderPort, matchingRecords);
			continue;
		}
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
			if (addressIsLocal(sender)) scope = "Local";
		}

		// Process the records locally
		acceptRecords(Record::listFromBytes(datagram), removeThem, scope, sender.toString(), QString::number(senderPort));

		// Normal: Respond to only Local requests with only owned Records
		if (respond and not mServerMode and (scope=="Local" or scope=="Loopback"))
		{
			globalSocket->writeDatagram(QByteArray("DSDA")+makeDatagram(ownedRecords, true), sender, senderPort);
		}
		// Server: Respond to only global requests with owned and found Records
		else if (respond and mServerMode and scope=="Global")
		{
			globalSocket->writeDatagram(QByteArray("DSDA")+makeDatagram(ownedRecords + foundRecords.keys(), false), sender, senderPort);
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
		recordsToSend.push_back(record);
	}
	if (not recordsToSend.empty() or defaultScope=="Global")
	{
		// This is a discovery request which should be responded to
		QByteArray datagram("DSDR");
		if (departure) datagram = "DSDD";
		datagram += makeDatagram(recordsToSend, false);

		// Send to each Global server
		for (QPair<QHostAddress,quint16> globalServer : globalServers)
		{
			globalSocket->writeDatagram(datagram, globalServer.first, globalServer.second);
		}
	}

	///////////////////////////////////////////////////////////////////////
	// Local scope next
	///////////////////////////////////////////////////////////////////////
	for (Record record : ownedRecords)
	if (record["Scope"] == "Local")
	{
		recordsToSend.push_back(record);
	}
	if (not recordsToSend.empty() or defaultScope=="Global" or defaultScope=="Local")
	{
		QByteArray datagram("DSDA");
		if (announceNeedsResponse) datagram = "DSDR";
		if (departure)             datagram = "DSDD";
		datagram += makeDatagram(recordsToSend, true);

		// For each interface
		for (auto iface : QNetworkInterface::allInterfaces())
		{
			// For each IPv4 address on this interface
			for (auto entry : iface.addressEntries())
			{
				if (entry.ip().protocol() != QAbstractSocket::IPv4Protocol) continue;

				// Add it to addressEntryCache if necessary for local scope testing
				if (not addressEntryCache.contains(entry)) addressEntryCache.push_back(entry);

				// Multicast
				if (iface.flags().testFlag(QNetworkInterface::CanMulticast) and !entry.ip().isNull() and !entry.ip().isLoopback())
				{
					// Create the socket if it doesn't exit yet
					if (not multiSocket.contains(entry.ip().toString()))
					{
						logDebug(QString("New multicast socket: %1 %2").arg(entry.ip().toString(), entry.netmask().toString()));

						// Add it, create and bind the socket
						QUdpSocket* socket = new QUdpSocket(this);
						connect(socket, SIGNAL(readyRead()), this, SLOT(readDatagrams()));
						socket->setSocketOption(QAbstractSocket::MulticastTtlOption, 1);
						if (not socket->bind(QHostAddress::AnyIPv4, port, QAbstractSocket::ShareAddress | QAbstractSocket::ReuseAddressHint))
						{
							logWarning(QString("Error binding to iface %1: %2").arg(entry.ip().toString(), socket->errorString()));
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
	// Loopback scope last
	///////////////////////////////////////////////////////////////////////
	for (Record record : ownedRecords)
	if (record["Scope"] == "Loopback")
	{
		recordsToSend.push_back(record);
	}
	if (not recordsToSend.empty() or true) // Any scope is above or equivalent to loopback
	{
		QByteArray datagram("DSDA");
		if (announceNeedsResponse) datagram = "DSDR";
		if (departure)             datagram = "DSDD";
		datagram += makeDatagram(recordsToSend, true);

		// Loopback
		//loopbackSocket->writeDatagram(datagram, QHostAddress::LocalHost, port);
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
			logDebug(QString("Lost Record: %1").arg(pair.first.toString()));
			foundRecords.remove(pair.first);

			// Notify this app
			emit recordLost(pair.first);

			// Server: Forward global departures to other global peers
			if (mServerMode)
			{
				QList<Record> recordInListForm;
				recordInListForm.append(pair.first);

				QByteArray datagram("DSDD");
				datagram += makeDatagram(recordInListForm, false);

				for (Record& record : foundRecords.keys())
				if (record["Scope"] == "Global")
				{
					bool ok;
					globalSocket->writeDatagram(datagram, QHostAddress(record["Address"]), record["Port"].toUInt(&ok, 10));
				}
			}

		}
		else if (delta < next) next = delta; // Set the timer for this one
	}
	// Set timer for next time
	expireTimer->start(next);
}

void Discover::acceptRecords (QList<Record> records, bool removeThem, QString senderScope, QString senderAddress, QString senderPort)
{
	// TODO
	
	// This function gets its own copy of records
	// This loop edits each item so it can be used after
	for (Record& record : records)
	{
		// Preprocess the record before considering it
		Record::decompressReserved(record);

		// Set the scope based on the address if included, otherwise scope of sender
		QHostAddress recordAddress;
		if (record.has("Address")) recordAddress.setAddress(record["Address"]);
		if (recordAddress.protocol()!=QAbstractSocket::IPv4Protocol) recordAddress = QHostAddress::Null;
		if (recordAddress==QHostAddress::Broadcast) recordAddress = QHostAddress::Null;
		if (recordAddress==QHostAddress::AnyIPv4) recordAddress = QHostAddress::Null;
		if (recordAddress.isNull())
		{
			record["Scope"] = senderScope;
		}
		else
		{
				 if (recordAddress.isLoopback())    record["Scope"] = "Loopback";
			else if (addressIsLocal(recordAddress)) record["Scope"] = "Local";
			else                                    record["Scope"] = "Global";
		}

		if (record["Address"].isEmpty()) record["Address"] = senderAddress; // Sender may override these
		if (record["Port"].isEmpty()) record["Port"] = senderPort; // Sender may override these
		// TODO Extract a custom expiration duration so each record can have its own


		if (removeThem)
		{
			// See if it's new
			if (foundRecords.contains(record))
			{
				logDebug(QString("Record Departed: %1").arg(record.toString()));
				foundRecords.remove(record);
				emit recordLost(record);
			}
		}
		else
		{
			// See if it's new
			if (not foundRecords.contains(record) and passesFilters(record))
			{
				logDebug(QString("Found Record: %1").arg(record.toString()));
				emit recordFound(record);
			}

			// Reset the timer
			foundRecords[record] = QDateTime::currentMSecsSinceEpoch() + 2500; // Set the expire time
		}

		expireTimer->start(0); // In case the next expiration time was changed
	}
	
	// Server: Forward global departures to other global peers
	if (mServerMode and removeThem and senderScope == "Global")
	{
		QByteArray datagram("DSDD");
		datagram += makeDatagram(records, false);
		
		for (Record& record : foundRecords.keys())
		if (record["Scope"] == "Global")
		{
			bool ok;
			globalSocket->writeDatagram(datagram, QHostAddress(record["Address"]), record["Port"].toUInt(&ok, 10));
		}
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

bool Discover::addressIsLocal(QHostAddress address)
{
	quint32 senderInt = address.toIPv4Address();
	for (auto entry : addressEntryCache)
	if (entry.ip().protocol() == QAbstractSocket::IPv4Protocol)
	{
		quint32 netmaskInt = entry.netmask().toIPv4Address();
		if (netmaskInt==0 or netmaskInt==4294967295) continue; // Netmask isn't valid, either 0.0.0.0 or 255.255.255.255
		// #BUG1
		quint32 entryInt = entry.ip().toIPv4Address();
		if ((senderInt & netmaskInt) == (entryInt & netmaskInt))
		{
			// Came from the local network
			return true;
		}
	}
	return false;
}

QByteArray Discover::makeDatagram (QList<Record> records, bool injectPort)
{
	for (Record& record : records)
	{
		// Remove Scope because it will be changed on receipt anyway
		record.remove("Scope");

		// Inject main socket's port number if necessary
		if (injectPort and globalSocket and !record.has("Port")) record["Port"] = QString::number(globalSocket->localPort());

		// Compress reserved names
		Record::compressReserved(record);
	}
	return Record::listToBytes(records);
}




