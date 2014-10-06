#ifndef DISCOVER_H
#define DISCOVER_H

#include <QObject>
#include <QTimer>
#include <QHostInfo>
#include <QDateTime>
#include <iostream>
#include <map>
#include "Messages.h"
#include "Config.h"

class Discover : public QObject
{
    Q_OBJECT
//    Q_PROPERTY(QHostAddress peerAddress READ peerAddress WRITE setPeerAddress NOTIFY peerAddressChanged)
//    Q_PROPERTY(quint16 peerPort READ peerPort WRITE setPeerPort NOTIFY peerPortChanged)


public:
    explicit Discover (QObject *parent = 0)
    {

    }

    void start (QString role)
    {
        // Configuration
        Config& config = Config::getSingleton();
        QString towerString(config.value("Tower Host").toString());
        quint16 towerPort = config.value("Tower Port").toInt();
        int channel = config.value("Channel").toInt();

        // TODO Get most of this stuff from configuration
        mRole = role;
        mChannel = channel;

        mTowerString = towerString;
        mTowerAddress = QHostAddress(mTowerString);
        mTowerPort = towerPort;

        mLastPingId = 0;

        // If the tower IP is defined, signal it now, otherwise signal after DNS lookup
        if (not mTowerAddress.isNull()) emit towerChanged(mTowerAddress, mTowerPort);

        // Start the first update now
        update();

        // Set a periodic timer for proceeding updates
        mTimer = new QTimer(this);
        connect(mTimer, SIGNAL(timeout()), this, SLOT(update()));
        mTimer->start(1000); // TODO Configure frequency
    }

signals:
    void towerChanged (QHostAddress address, quint16 port);
    void peerChanged (QHostAddress address, quint16 port);
    void sendDatagram (QByteArray datagram, QString messageType, QHostAddress address, quint16 port);

public slots:
    void hostResults (QHostInfo info)
    {
        if (info.error() == QHostInfo::NoError and info.addresses().size()>0)
        {
            // Only pay attention if the DNS returned a different address
            if (info.addresses().first() != mTowerAddress)
            {
                mTowerAddress = info.addresses().first();
                //std::cout << "Tower hostname resolved: " << mTowerAddress.toString().toStdString() << std::endl;
                emit towerChanged(mTowerAddress, mTowerPort);
                emit update();
            }
        }
        else
        {
            // The timer will make it try again soon
            std::cout << "Error resolving tower hostname: " << mTowerString.toStdString() << std::endl;
        }
    }
    void update ()
    {
        // If this is the Control, initiate a 3-trip ping with the Flight
        if (not mPeerAddress.isNull() and mRole == "Control")
        {
            Ping ping;
            ping.id = ++mLastPingId;
            ping.trips = 1;
            pingTimes[ping.id] = QDateTime::currentDateTime();
            emit sendDatagram(encodeMessage<Ping>(ping), "Ping", mPeerAddress, mPeerPort);
        }

        // Send a request to the Tower
        if (not mTowerAddress.isNull())
        {
            // Address of tower has been given or resolved from hostname
            DiscoverRequest request;
            request.role = mRole;
            request.channel = mChannel;
            emit sendDatagram(encodeMessage<DiscoverRequest>(request), "DiscoverResponse", mTowerAddress, mTowerPort);
        }
        else
        {
            // Try to resolve the hostname string
            QHostInfo::lookupHost(mTowerString, this, SLOT(hostResults(QHostInfo)));
        }
    }
    void receiveResponse (DiscoverResponse response, QHostAddress address, quint16 port)
    {
        // TODO Validate that the address and port for this response match the expected ones
        //      (either choose to listen or ignore)

        if (response.sec_ago < 0)
        {
            // Peer has not checked in yet
        }
        else
        {
            // Peer has checked in
            // Check if any info has changed and emit the signal if so
            bool changed = false;
            if (response.address != mPeerAddress) { mPeerAddress = response.address; changed = true; }
            if (response.port != mPeerPort) { mPeerPort = response.port; changed = true; }
            if (changed) emit peerChanged(mPeerAddress, mPeerPort);
        }
    }
    void receivePing (Ping ping, QHostAddress address, quint16 port)
    {
        // TODO Validate that the address and port for this response match the expected ones
        //      (either choose to listen or ignore)

        // TODO Possibly do pinging with the Tower as well

        // TODO Count the time since the last ping with this id
        if (pingTimes.find(ping.id) == pingTimes.end())
        {
            // Send time is not recorded yet, do it
            pingTimes[ping.id] = QDateTime::currentDateTime();
        }
        else
        {
            // Send time is recorded, ping is the difference between now and then
            float dif = 1e-3*pingTimes[ping.id].msecsTo(QDateTime::currentDateTime());
            std::cout << "Ping: " << 1e3*dif << " ms" << std::endl;
        }

        // Respond to the ping if it has not made 3 trips yet
        if (not mPeerAddress.isNull() and ping.trips < 3)
        {
            ++ping.trips;
            emit sendDatagram(encodeMessage<Ping>(ping), "Ping", mPeerAddress, mPeerPort);
        }
    }

public:
    QHostAddress mTowerAddress, mPeerAddress;
    quint16 mTowerPort, mPeerPort;

private:
    QTimer* mTimer;
    QString mTowerString, mRole;
    int mChannel;
    int mLastPingId;
    std::map<int,QDateTime> pingTimes;
};

#endif // DISCOVER_H
