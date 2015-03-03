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

	// Try get only real interfaces first
	for (auto iface : ifaces)
    {
        QString name = iface.name();
        QString hName = iface.humanReadableName();
        QString addr = iface.hardwareAddress();
        if (addr == "00:00:00:00:00:00") addr = "";
        if (addr == "00:00:00:00:00:00:00:E0") addr = "";

        if (not addr.isEmpty() and
                (name.startsWith("en") or // OS X and new linux
                 name.startsWith("wlo") or // New linux
                 name.startsWith("eth") or // Old Linux
                 name.startsWith("wlan") or // Old Linux
                 hName.startsWith("Wi-Fi") or // Windows
                 hName.startsWith("Ethernet") // Windows
                 ))
		{
            qDebug() << "Real iface:" << name << hName << addr;
			data += addr;
		}
        else qDebug() << "Ignoring iface:" << name << hName << addr;
	}

    // Behavior change: If there weren't positive results, don't return anything, skip the next step
    if (data.isEmpty()) return "";

	// If no real interfaces, combine all available
	if (data.isEmpty())
	{
		for (auto iface : ifaces)
		{
            QString name = iface.name();
            QString hName = iface.humanReadableName();
			QString addr = iface.hardwareAddress();
			if (addr == "00:00:00:00:00:00") addr = "";
            if (addr == "00:00:00:00:00:00:00:E0") addr = "";

            qDebug() << "Fallback iface:" << name << hName << addr;
			data += addr;
		}
	}
	qDebug() << "";

	// No hardware addresses?
	if (data.isEmpty()) return "";

	// Hash and cut a piece
	data += salt;
	return QCryptographicHash::hash(data, QCryptographicHash::Sha1).toBase64().left(6); // 6 base64 characters = 36 bits
}
