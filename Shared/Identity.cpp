#include "Identity.h"
#include <QNetworkInterface>
#include <QCryptographicHash>

Identity::Identity ()
{
}

QString Identity::basedOnInterfaces (QByteArray salt)
{
	auto ifaces = QNetworkInterface::allInterfaces();

	QByteArray data;
	for (auto iface : ifaces)
	{
		qDebug() << "Iface:" << iface.name() << iface.hardwareAddress();
		data += iface.hardwareAddress();
	}

	// No hardware addresses?
	if (data.isEmpty()) return "";

	// Hash and cut a piece
	data += salt;
	return QCryptographicHash::hash(data, QCryptographicHash::Sha1).toBase64().left(6); // 6 base64 characters = 36 bits
}
