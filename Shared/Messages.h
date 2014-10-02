#ifndef MESSAGES_H
#define MESSAGES_H

#include <QHostAddress>
#include <QJsonDocument>
#include <QJsonObject>
#include <QByteArray>
#include <iostream>

// Check the magic string at the beginning of the datagram
template <class Message>
bool isMessage (const QByteArray& datagram)
{
    return datagram.startsWith(Message::magic());
}

// Check and trim the magic string from the beginning then decode it
template <class Message>
Message decodeMessage (const QByteArray& datagram)
{
    // TODO Consider using exceptions
    if (not isMessage<Message>(datagram)) return Message();
    return Message::decode(QByteArray(
                               datagram.data() + Message::magic().size(),
                               datagram.size() - Message::magic().size()
                               ));
}

// Add the magic string to the beginning of the encoded message
template <class Message>
QByteArray encodeMessage (const Message& message)
{
    QByteArray datagram = Message::magic();
    datagram.append(message.encode());
    return datagram;
}

struct DiscoverRequest
{
    QString role; // Role of sender
    int channel;  // To allow the server to match multiple sets of Flight/Control at a time

    static QByteArray magic () { return QByteArray("DiscoverRequest1"); }
    static DiscoverRequest decode (const QByteArray& data)
    {
        DiscoverRequest message;
        QJsonObject json = QJsonDocument::fromBinaryData(data).object();
        message.role = json["Role"].toString();
        message.channel = json["Channel"].toDouble();
        return message;
    }
    QByteArray encode () const
    {
        QJsonObject json;
        json["Role"] = role;
        json["Channel"] = channel;
        return QJsonDocument(json).toBinaryData();
    }
};
struct DiscoverResponse
{
    QHostAddress address;
    quint16 port;
    float sec_ago;

    static QByteArray magic () { return QByteArray("DiscoverResponse1"); }
    static DiscoverResponse decode (const QByteArray& data)
    {
        DiscoverResponse message;
        QJsonObject json = QJsonDocument::fromBinaryData(data).object();
        message.address = QHostAddress(json["Address"].toString());
        message.port = json["Port"].toDouble();
        message.sec_ago = json["SecAgo"].toDouble();
        return message;
    }
    QByteArray encode () const
    {
        QJsonObject json;
        json["Address"] = address.toString();
        json["Port"] = port;
        json["SecAgo"] = sec_ago;
        return QJsonDocument(json).toBinaryData();
    }
};
struct Ping
{
    int id;
    int trips;

    static QByteArray magic () { return QByteArray("Ping1"); }
    static Ping decode (const QByteArray& data)
    {
        Ping message;
        QJsonObject json = QJsonDocument::fromBinaryData(data).object();
        message.id = json["Id"].toDouble();
        message.trips = json["Trips"].toDouble();
        return message;
    }
    QByteArray encode () const
    {
        QJsonObject json;
        json["Id"] = id;
        json["Trips"] = trips;
        return QJsonDocument(json).toBinaryData();
    }
};

#endif // MESSAGES_H
