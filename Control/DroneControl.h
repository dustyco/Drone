#ifndef DRONECONTROL_H
#define DRONECONTROL_H

#include <QObject>
#include <QTimer>
#include <QUdpSocket>
#include <iostream>
#include <../Shared/Discover.h>
#include <../Shared/Messages.h>
#include <../Shared/Config.h>

class DroneControl : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString networkStatus READ networkStatus WRITE setNetworkStatus NOTIFY networkStatusChanged)
	Q_PROPERTY(long traffic READ traffic NOTIFY trafficChanged)
	Q_PROPERTY(long ping READ ping NOTIFY pingChanged)
	Q_PROPERTY(qreal frameTrigger READ frameTrigger WRITE setFrameTrigger) // See setFrameTrigger()

public:
	explicit DroneControl (QObject *parent = 0);

	long traffic () const;
	long ping () const;

	QString networkStatus () const;
	void setNetworkStatus (QString networkStatus);

    // Hijack QML's property animation to get periodic vsync aligned signals
	qreal frameTrigger () const;
	void setFrameTrigger (qreal frameTrigger);

private:
	QString checkConfig(); // Returns an error string, empty indicates no error

signals:
	void networkStatusChanged (QString);
	void trafficChanged (long);
	void pingChanged (long);

public slots:
	void recordFound (Record record);
	void recordLost (Record record);
	void gotDatagram (QByteArray datagram, QHostAddress sender, quint16 senderPort, QList<Record> matchingRecords);

private:
	Discover* mDiscover;
	QString mNetworkStatus;
	int i;
	long mTraffic;
	long mPing;

	void doNetworkStatus ();
};

#endif // DRONECONTROL_H
