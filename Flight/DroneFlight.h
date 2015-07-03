#ifndef DRONEFLIGHT_H
#define DRONEFLIGHT_H

#include <QObject>
#include <QTimer>
#include <QUdpSocket>
#include "../Shared/Discover.h"

class DroneFlight : public QObject
{
    Q_OBJECT

public:
	explicit DroneFlight (QObject *parent = 0);

private:
	QString checkConfig(); // Returns an error string, empty indicates no error

signals:

public slots:
	void recordFound (Record record);
	void recordLost (Record record);
	void gotDatagram (QByteArray datagram, QHostAddress sender, quint16 senderPort, QList<Record> matchingRecords);

private:
    Discover* mDiscover;
    QTimer mTimer;
    QUdpSocket* mSocket;
};

#endif // DRONEFLIGHT_H
