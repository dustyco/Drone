#ifndef DISCOVER_H
#define DISCOVER_H

#include <QObject>
#include <QMap>
#include <QHostAddress>
#include <QNetworkAddressEntry>
#include <QTimer>
#include <QUdpSocket>
#include "Record.h"

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

class Discover : public QObject
{
	Q_OBJECT

public:
	explicit Discover (QObject *parent = 0, bool serverMode = false);
	~Discover ();
	void addGlobalServer (QString globalServer);
	void addFilter (QString key, QString value);
	void addRecord (Record record);
	void setGlobalServerMode (bool b);
	bool start ();

signals:
	void recordFound (Record record);
	void recordLost (Record record);
	void status (QString status);

private slots:
	void readDatagrams ();
	void announceRecords ();
	void expireRecords ();
	void acceptRecords (QList<Record> records, bool removeThem, QString senderScope, QString senderAddress, QString senderPort);

private:
	bool passesFilters (Record record);
	bool addressIsLocal (QHostAddress address);
	
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
	QSet<QHostAddress> globalServers;
};

#endif // DISCOVER_H
