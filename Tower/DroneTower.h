#ifndef DRONETOWER_H
#define DRONETOWER_H

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

class DroneTower : public QObject
{
    Q_OBJECT

    struct Info
    {
        QHostAddress address;
        quint16 port;
        QDateTime last;

        Info () : port(0) {}
    };
    struct InfoPair
    {
        Info flight, control;
    };

public:
    explicit DroneTower (QObject *parent = 0)
    {
        // Load the configuration
        Config& config = Config::getSingleton();
        config.prefix = "Tower/";
        quint16 port = config.value("Port").toInt();

        // Set up network socket
        mSocket = new QUdpSocket(this);
        connect(mSocket, SIGNAL(readyRead()), this, SLOT(readDatagrams()));
        if (not mSocket->bind(QHostAddress::Any, port))
            std::cout << "Failed to bind to port " << port << std::endl;
        else std::cout << "Listening on port " << port << std::endl;
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
            std::cout << sender.toString().toStdString() << ":" << senderPort << " - " << datagram.size() << " Bytes - ";

            // Process datagram
            if (isMessage<DiscoverRequest>(datagram))
            {
                DiscoverRequest request = decodeMessage<DiscoverRequest>(datagram);
                std::cout << "DiscoverRequest(role: " << request.role.toStdString() << ", channel: " << request.channel << " )" << std::endl;

                InfoPair& pair = mInfo[request.channel];
                Info* requester, * partner;
                if (request.role == "Flight")
                {
                    requester = &pair.flight;
                    partner = &pair.control;
                }
                else if (request.role == "Control")
                {
                    requester = &pair.control;
                    partner = &pair.flight;
                }
                else
                {
                    std::cout << "Unexpected role: " << request.role.toStdString() << std::endl;
                    continue;
                }

                // Update requester's information
                requester->address = sender;
                requester->port = senderPort;
                requester->last = QDateTime::currentDateTime();

                // Debug
                std::cout << "From: " << requester->address.toString().toStdString() << ":" << senderPort;
                std::cout << "   Partner: " << partner->address.toString().toStdString() << ":" << partner->port << std::endl;

                // Respond with partner's information
                DiscoverResponse response;
                if (partner->last.isValid())
                {
                    // The partner is checked in, send valid info including sec_ago>=0
                    response.address = partner->address;
                    response.port = partner->port;
                    response.sec_ago = 1e-3*partner->last.msecsTo(requester->last);
                    if (response.sec_ago < 0) response.sec_ago = 0;
                }
                else
                {
                    // Partner has not yet checked in, send null info, indicated by sec_ago<0
                    response.sec_ago = -1;
                }
                mSocket->writeDatagram(encodeMessage<DiscoverResponse>(response), sender, senderPort);
            }
        }
    }

private:
    QUdpSocket* mSocket;
    std::map<int,InfoPair> mInfo;
};

#endif // DRONETOWER_H
