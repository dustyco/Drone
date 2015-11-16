#include "SensorIface.h"
#include "../Shared/Config.h"

SensorIface::SensorIface (QObject* parent, Discover* discover) : QObject(parent)
{
	setLoggerTag("SensorIface");
	mMode = ErrorMode;
	mTTYTimer = new QTimer(this);
	mDiscover = discover;
}

bool SensorIface::start ()
{
	// Read the mode
	Config& config = Config::getSingleton();
	QString modeString = config.value("Sensor Mode").toString();
		 if (modeString == "TTY")      mMode = TTYMode;
	else if (modeString == "Discover") mMode = DiscoverMode;
	else
	{
		mMode = ErrorMode;
		logWarning(QString("Invalid Sensor Mode in config: %1").arg(modeString));
		// TODO Fatal
		return false;
	}

	// Start TTY mode
	if (mMode == TTYMode)
	{
		// Validate configuration
		QString description = config.value("Sensor TTY").toString();
		logInfo(QString("Sensor Mode: TTY %1").arg(description));

		// Start querying
		mTTYTimer->start(0);
	}

	// Start Discover mode
	if (mMode == DiscoverMode)
	{
		// Validate configuration
		QString description = config.value("Sensor Discover").toString();
		logInfo(QString("Sensor Mode: Discover %1").arg(description));

		// Start querying
	}

	return true;
}

void SensorIface::ttyQuery()
{
	if (mMode != TTYMode)
	{
		mTTYTimer->stop();
		return;
	}
	mTTYTimer->start(250);

	QList<QSerialPortInfo> serialPortInfoList = QSerialPortInfo::availablePorts();

	const QString blankString = QObject::tr("<Blank>");
	QString description;
	QString manufacturer;
	QString serialNumber;

	foreach (const QSerialPortInfo &serialPortInfo, serialPortInfoList) {
		if (ttyMatch(serialPortInfo, Config::getSingleton().value("Sensor TTY").toString()))
		{
			// Match!
		}

		description = serialPortInfo.description();
		manufacturer = serialPortInfo.manufacturer();
		serialNumber = serialPortInfo.serialNumber();
		out << endl
			<< QObject::tr("        Name: ") << serialPortInfo.portName() << endl
			<< QObject::tr("    Location: ") << serialPortInfo.systemLocation() << endl
			<< QObject::tr(" Description: ") << (!description.isEmpty() ? description : blankString) << endl
			<< QObject::tr("Manufacturer: ") << (!manufacturer.isEmpty() ? manufacturer : blankString) << endl
			<< QObject::tr("      Serial: ") << (!serialNumber.isEmpty() ? serialNumber : blankString) << endl
			<< QObject::tr("    VendorID: ") << (serialPortInfo.hasVendorIdentifier() ? QByteArray::number(serialPortInfo.vendorIdentifier(), 16) : blankString) << endl
			<< QObject::tr("   ProductID: ") << (serialPortInfo.hasProductIdentifier() ? QByteArray::number(serialPortInfo.productIdentifier(), 16) : blankString) << endl
			<< QObject::tr("        Busy: ") << (serialPortInfo.isBusy() ? QObject::tr("Yes") : QObject::tr("No")) << endl;
	}
}

bool SensorIface::ttyMatch(const QSerialPortInfo& serialPortInfo, QString description)
{

}

