#ifndef DRONECONTROL_H
#define DRONECONTROL_H

#include <QObject>
#include <QTimer>
#include <QUdpSocket>
#include <iostream>
#include <../Shared/Discover.h>
#include <../Shared/Messages.h>
#include <../Shared/Config.h>

class DroneDiscoverView : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString networkStatus READ networkStatus WRITE setNetworkStatus NOTIFY networkStatusChanged)
    Q_PROPERTY(long traffic READ traffic NOTIFY trafficChanged)
    Q_PROPERTY(long ping READ ping NOTIFY pingChanged)
    Q_PROPERTY(qreal frameTrigger READ frameTrigger WRITE setFrameTrigger) // See setFrameTrigger()

public:
    explicit DroneDiscoverView (QObject *parent = 0)
    {
		// Print the version
		QString version(GIT_VERSION);
		std::cout << "DroneControl " << version.toStdString() << std::endl;

        // Load the configuration
        Config& config = Config::getSingleton();
        config.prefix = "Control/";
        quint16 port = config.value("Port").toInt();

        i = 0;
//        connect(&mTimer, SIGNAL(timeout()), this, SLOT(update()));
        mTimer.start(500);

        // Set up socket
        mSocket = new QUdpSocket(this);
        connect(mSocket, SIGNAL(readyRead()), this, SLOT(readDatagrams()));
        if (not mSocket->bind(QHostAddress::Any, port))
            std::cout << "Failed to bind to port " << port << std::endl;
        else std::cout << "Using port " << port << std::endl;

        mTraffic = 0;
        mPing = 0;

        // Start peer discovery (may send first signal immediately, resolve deps first
        mDiscover = new Discover(this);
        connect(mDiscover, &Discover::sendDatagram, this, &DroneDiscoverView::sendDatagram);
        connect(mDiscover, &Discover::towerChanged, this, &DroneDiscoverView::towerChanged);
        connect(mDiscover, &Discover::peerChanged, this, &DroneDiscoverView::peerChanged);
        mDiscover->start("Control");
    }

    long traffic () const { return mTraffic; }
    long ping () const { return mPing; }

    QString networkStatus () const { return mNetworkStatus; }
    void setNetworkStatus (QString networkStatus)
    {
        if (networkStatus != mNetworkStatus)
        {
            mNetworkStatus = networkStatus;
            emit networkStatusChanged(mNetworkStatus);
        }
    }

    // Hijack QML's property animation to get periodic vsync aligned signals
    qreal frameTrigger () const { return 0; }
    void setFrameTrigger (qreal frameTrigger)
    {
        frameTrigger;
        // Check joystick input and send a packet if
        //   Joystick has moved
        //   Heartbeat interval has passed
        //   Any addresses have changed (maybe make these properties instead)
    }

signals:
    void networkStatusChanged (QString);
    void trafficChanged (long);
    void pingChanged (long);

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
            logTrafficIn(type, datagram.size());

        }
    }

    void towerChanged (QHostAddress address, quint16 port)
    {
        std::cout << "Tower address changed (" << address.toString().toStdString() << ":" << port << ")" << std::endl;
        doNetworkStatus();
    }
    void peerChanged (QHostAddress address, quint16 port)
    {
        std::cout << "Peer address changed (" << address.toString().toStdString() << ":" << port << ")" << std::endl;
        doNetworkStatus();
    }

    void sendDatagram (QByteArray datagram, QString messageType, QHostAddress address, quint16 port)
    {
        logTrafficOut(messageType, datagram.size());
        mSocket->writeDatagram(datagram, address, port);
    }

private:
    Discover* mDiscover;
    QString mNetworkStatus;
    QTimer mTimer;
    QUdpSocket* mSocket;
    QTimer mTowerTimer;
    int i;
    long mTraffic;
    long mPing;

    void doNetworkStatus ()
    {
        QString status;
        status += QString("Tower: ") + mDiscover->mTowerAddress.toString() + QString(":") + QString::number(mDiscover->mTowerPort);
        status += QString("\n");
        if (mDiscover->mPeerAddress.isNull())
            status += QString("Peer: Discovering...");
        else
            status += QString("Peer: ") + mDiscover->mPeerAddress.toString() + QString(":") + QString::number(mDiscover->mPeerPort);
        setNetworkStatus(status);
    }
    void logTrafficIn (QString messageType, long messageSize)
    {
        mTraffic += messageSize;
        emit trafficChanged(mTraffic);
    }
    void logTrafficOut (QString messageType, long messageSize)
    {
        mTraffic += messageSize;
        emit trafficChanged(mTraffic);
    }
};

#endif // DRONECONTROL_H
