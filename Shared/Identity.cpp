#include "Identity.h"
#include <QNetworkInterface>
#include <QCryptographicHash>

Identity::Identity ()
{
}

QString Identity::basedOnInterfaces ()
{
	auto ifaces = QNetworkInterface::allInterfaces();

	QByteArray data;
	for (auto iface : ifaces)
	if (true) // not iface.hardwareAddress().isEmpty()
	{
		qDebug() << "Iface:" << iface.name() << iface.hardwareAddress();
		data += iface.hardwareAddress();
	}

	if (data.isEmpty()) return "";
	else                return QCryptographicHash::hash(data, QCryptographicHash::Md5).toBase64();
}
