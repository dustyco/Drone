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

class DroneFlight : public QObject
{
    Q_OBJECT

public:
    explicit DroneFlight (QObject *parent = 0)
    {
//        connect(&mTimer, SIGNAL(timeout()), this, SLOT(update()));
        mTimer.start(500);

        // Set up socket
        quint16 port = 28052;
        mSocket = new QUdpSocket(this);
        connect(mSocket, SIGNAL(readyRead()), this, SLOT(readDatagrams()));
        if (not mSocket->bind(QHostAddress::Any, port))
            std::cout << "Failed to bind to port " << port << std::endl;
        else std::cout << "Using port " << port << std::endl;

        //mTraffic = 0;

        // Start peer discovery (may send first signal immediately, resolve deps first
        mDiscover = new Discover(this);
        connect(mDiscover, &Discover::sendDatagram, this, &DroneFlight::sendDatagram);
        connect(mDiscover, &Discover::towerChanged, this, &DroneFlight::towerChanged);
        connect(mDiscover, &Discover::peerChanged, this, &DroneFlight::peerChanged);
        mDiscover->start("Flight", 1, "76.14.55.211", 28050);
    }

signals:

public slots:
    void readDatagrams ()
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
            //logTrafficIn(type, datagram.size());

        }
    }

    void towerChanged (QHostAddress address, quint16 port)
    {
        std::cout << "Tower address changed (" << address.toString().toStdString() << ":" << port << ")" << std::endl;
        //doNetworkStatus();
    }
    void peerChanged (QHostAddress address, quint16 port)
    {
        std::cout << "Peer address changed (" << address.toString().toStdString() << ":" << port << ")" << std::endl;
        //doNetworkStatus();
    }

    void sendDatagram (QByteArray datagram, QString messageType, QHostAddress address, quint16 port)
    {
        //logTrafficOut(messageType, datagram.size());
        mSocket->writeDatagram(datagram, address, port);
    }

private:
    Discover* mDiscover;
    QTimer mTimer;
    QUdpSocket* mSocket;
};

#endif // DRONEFLIGHT_H
