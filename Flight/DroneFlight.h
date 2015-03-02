#ifndef DRONEFLIGHT_H
#define DRONEFLIGHT_H

#include <QObject>
#include <QTimer>
#include <QUdpSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDateTime>
#include <QHostInfo>
#include <iostream>
#include <map>
#include "../Shared/Messages.h"
#include "../Shared/Discover.h"
#include "../Shared/Config.h"

#include "../Shared/Identity.h"

class DroneFlight : public QObject
{
    Q_OBJECT

public:
	explicit DroneFlight (QObject *parent = 0);

signals:

public slots:
	void readDatagrams ();
	void towerChanged (QHostAddress address, quint16 port);
	void peerChanged (QHostAddress address, quint16 port);
	void sendDatagram (QByteArray datagram, QString messageType, QHostAddress address, quint16 port);

private:
    Discover* mDiscover;
    QTimer mTimer;
    QUdpSocket* mSocket;
};

#endif // DRONEFLIGHT_H
