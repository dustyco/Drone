

#include "DroneDiscoverView.h"
#include <iostream>


DroneDiscoverView::DroneDiscoverView (QObject *parent) : QObject(parent)
{
	// Print the version
	QString version(GIT_VERSION);
	qDebug() << "DroneDiscoverView" << version;

	// Load the configuration
	Config& config = Config::getSingleton();
	config.prefix = "DiscoverView/";
	QString configError = checkConfig();
	if (not configError.isEmpty())
	{
		qDebug() << "Config error:" << configError;
	}
	QHostAddress towerAddress(config.value("Tower Address").toString());
	quint16 towerPort = config.value("Tower Port").toInt();

	QHostAddress multicastAddress(config.value("Multicast Address").toString());
	quint16 multicastPort = config.value("Multicast Port").toInt();

	// Start peer discovery
	Record record;
	record["Service"] = "DroneDiscoverView";
	record["Scope"] = "Global";

	mDiscover = new Discover(this, Discover::NormalMode, multicastAddress, multicastPort);
	connect(mDiscover, SIGNAL(recordFound(Record)), this, SLOT(recordFound(Record)));
	connect(mDiscover, SIGNAL(recordLost(Record)), this, SLOT(recordLost(Record)));
	mDiscover->addRecord(record);
	//mDiscover->addFilter("Channel", config.value("Channel").toString());
	mDiscover->addGlobalServer(towerAddress, towerPort);
	mDiscover->start();
}

QString DroneDiscoverView::version() const
{
	return GIT_VERSION;
}

QString DroneDiscoverView::text () const
{
	QString groupBy("Machine");

	// Sort records by the instance
	QMap<QString,QList<QString> > lines;
	for (Record record : records.keys())
	{
		QString extra;
		QString instance = record[groupBy];
		if (records[record] >= 0)
		{
			extra += " (";
			extra += QString::number(records[record]);
			extra += ")";
		}
		record.remove(groupBy);
		lines[instance].push_back(record.toString() + extra);
	}

	// Print them
	QString ret;
	QString prefix;
	for (QString instance : lines.keys())
	{
		ret += prefix + groupBy + ": " + instance;

		for (QString line : lines[instance])
		{
			ret += QString("\n    ") + line;
		}
		ret += "\n";
		prefix = "\n";
	}

	return ret;
}

QString DroneDiscoverView::checkConfig()
{
	Config& config = Config::getSingleton();
	QString error("Error in DroneConfig.ini [DiscoverView]: ");

	// TODO Read the file ok

	QHostAddress towerAddress(config.value("Tower Address").toString());
	if (towerAddress.isNull()) return error + "Tower Address: " + config.value("Tower Address").toString();

	quint16 towerPort = config.value("Tower Port").toInt();
	if (towerPort < 1) return error + "Tower Port: " + config.value("Tower Port").toString();

	QHostAddress multicastAddress(config.value("Multicast Address").toString());
	if (multicastAddress.isNull()) return error + "Multicast Address: " + config.value("Multicast Address").toString();

	quint16 multicastPort = config.value("Multicast Port").toInt();
	if (multicastPort < 1) return error + "Multicast Port: " + config.value("Multicast Port").toString();

	return "";
}

void DroneDiscoverView::recordFound (Record record)
{
	if (records.contains(record))
	{
		// Exists, previously lost
		records[record] *= -1;
	}
	else
	{
		// New
		records[record] = 0;
	}
	emit textChanged(text());
}

void DroneDiscoverView::recordLost (Record record)
{
	records[record] += 1;
	records[record] *= -1;
	//qDebug() << "change:" << records[record];
	emit textChanged(text());
}
