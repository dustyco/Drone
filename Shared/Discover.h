#ifndef DISCOVER_H
#define DISCOVER_H

#include <QObject>
#include <QMap>
#include <QHostAddress>
#include <QNetworkAddressEntry>
#include <QTimer>
#include <QUdpSocket>
#include "Record.h"
#include "Logger.h"

/*
	Not global server mode:
	x-Announce only owned records periodically
	 -Store all found records until they exire
	 -Respond to only Local requests with
	  only owned Records from same scope
	
	Global server mode:
	x-Do not announce periodically
	 -Store only Global found Records until
	  they expire
	 -Respond to only Global requests with
	  Global owned and found Records
	
*/

class Discover : public QObject, public Logger
{
	Q_OBJECT

public:
	enum Mode { ServerMode, NormalMode };
	enum SendMode { AllRoutes };

signals:
	void recordFound (Record record);
	void recordLost (Record record);
	void gotDatagram (QByteArray datagram, QHostAddress sender, quint16 senderPort, QList<Record> matchingRecords);
	void status (QString status);

private slots:
	void readDatagrams ();
	void announceRecords ();
	void expireRecords ();
	void acceptRecords (QList<Record> records, bool removeThem, QString senderScope, QString senderAddress, QString senderPort);

public:
	explicit Discover (QObject *parent = 0, Mode mode = NormalMode, QHostAddress multicastAddress = QHostAddress(), quint16 multicastPort = 0);
	~Discover ();
	void addGlobalServerParse (QString globalServer);
	void addGlobalServer (QHostAddress address, quint16 port);
	void addFilter (QString key, QString value);
	void addRecord (Record record);
	void setGlobalServerMode (bool b);
	void setDefaultScope (QString scope); // Needed to request global records without owning any
	bool start ();
	bool sendDatagramTo (QByteArray datagram, Record filter, SendMode sendMode = AllRoutes);
	bool sendDatagramTo (QByteArray datagram, QHostAddress address, quint16 port);

private:
	bool passesFilters (Record record);
	bool addressIsLocal (QHostAddress address);
	QByteArray makeDatagram (QList<Record> records, bool injectPort);
	
private:
	QString defaultScope;
	QList<Record> ownedRecords;
	QMap<Record,qint64> foundRecords; // Second value is timestamp of last received
	bool running;
	bool mServerMode;
	bool announceNeedsResponse;
	bool departure;
	int announcePeriodMsec;
	int port;
	QHostAddress groupAddress;
	QTimer *timer;
	QTimer *expireTimer;
	QUdpSocket* loopbackSocket;
	QUdpSocket* globalSocket;
	QMap<QString, QUdpSocket*> multiSocket;
	QList<QNetworkAddressEntry> addressEntryCache;
	QMap<QString, QString> filters;
	QList<QPair<QHostAddress, quint16> > globalServers;
};

#endif // DISCOVER_H
