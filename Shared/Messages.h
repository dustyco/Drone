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
		QJsonObject json = QJsonDocument::fromJson(data).object();
		message.id = json["Id"].toDouble();
		message.trips = json["Trips"].toDouble();
		return message;
	}
	QByteArray encode () const
	{
		QJsonObject json;
		json["Id"] = id;
		json["Trips"] = trips;
		return QJsonDocument(json).toJson(QJsonDocument::Compact);
	}
};
struct ControlState
{
	unsigned char counter;
	float pitch, yaw, roll, throttle;

	static QByteArray magic () { return QByteArray::fromHex("656B"); } // Decimal: 25963
	static ControlState decode (const QByteArray& data)
	{
		ControlState message;
		message.counter = 0;
		message.pitch = message.yaw = message.roll = message.throttle = 0.0f;
		if (data.size() > 0) message.counter  = data[0];
		if (data.size() > 1) message.pitch    = float(data[1])/127.0f;
		if (data.size() > 2) message.yaw      = float(data[2])/127.0f;
		if (data.size() > 3) message.roll     = float(data[3])/127.0f;
		if (data.size() > 4) message.throttle = float(data[4])/127.0f;
		return message;
	}
	QByteArray encode () const
	{
		QByteArray result;
		result.append(counter);
		result.append(char(pitch*127.0f));
		result.append(char(yaw*127.0f));
		result.append(char(roll*127.0f));
		result.append(char(throttle*127.0f));
		return result;
	}
};
struct MotionState
{
	unsigned char counter;
	double lat, lon; // TODO Global position helper class
	//float pitch, yaw, roll; // TODO Orientation
	//Vec3 velocity; // TODO Use vectors

	static QByteArray magic () { return QByteArray::fromHex("656B"); } // Decimal: 25963
	static MotionState decode (const QByteArray& data)
	{
		// TODO
		Q_UNUSED(data);
		MotionState message;
		message.counter = 0;
//		message.pitch = message.yaw = message.roll = message.throttle = 0.0f;
//		if (data.size() > 0) message.counter  = data[0];
//		if (data.size() > 1) message.pitch    = float(data[1])/127.0f;
//		if (data.size() > 2) message.yaw      = float(data[2])/127.0f;
//		if (data.size() > 3) message.roll     = float(data[3])/127.0f;
//		if (data.size() > 4) message.throttle = float(data[4])/127.0f;
		return message;
	}
	QByteArray encode () const
	{
		QByteArray result;
		result.append(counter);
//		result.append(char(pitch*127.0f));
//		result.append(char(yaw*127.0f));
//		result.append(char(roll*127.0f));
//		result.append(char(throttle*127.0f));
		return result;
	}
};

//MotionState (Lat/Lon Centimeters, Orientation, Velocity)              2 + 8 + 3 + 3 = 16 bytes


#endif // MESSAGES_H
