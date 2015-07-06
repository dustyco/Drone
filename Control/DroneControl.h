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
	Q_PROPERTY(QString info READ info NOTIFY infoChanged)
	Q_PROPERTY(qreal frameTrigger READ frameTrigger WRITE setFrameTrigger) // See setFrameTrigger()

public:
	explicit DroneControl (QObject *parent = 0);

	QString info () const;

    // Hijack QML's property animation to get periodic vsync aligned signals
	qreal frameTrigger () const;
	void setFrameTrigger (qreal frameTrigger);

signals:
	void infoChanged (QString);

public slots:
	void recordFound (Record record);
	void recordLost (Record record);
	void gotDatagram (QByteArray datagram, QHostAddress sender, quint16 senderPort, QList<Record> matchingRecords);

private slots:
	QString checkConfig(); // Returns an error string, empty indicates no error
	void sendPing();

private:
	Discover* mDiscover;
	Record mFlightFilter; // Service and Channel of Flight peer for sending messages
	QString mInfo;
	QSet<QString> mFlightDescriptions;
	int mPingCounter;
	QTimer* mPingTimer;
	QMap<qint64, qint64> mPingTimes;
};

#endif // DRONECONTROL_H
