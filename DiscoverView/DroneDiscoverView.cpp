

#include "DroneDiscoverView.h"


DroneDiscoverView::DroneDiscoverView (QObject *parent) : QObject(parent)
{
	// Load the configuration
	Config& config = Config::getSingleton();
	config.prefix = "DiscoverView/";
	quint16 port = config.value("Port").toInt();
	Q_UNUSED(port);

	// Start peer discovery
	Record record;
	record["Service"] = "DroneDiscoverView";
	record["Scope"] = "Global";

	discover = new Discover(this);
	connect(discover, SIGNAL(recordFound(Record)), this, SLOT(recordFound(Record)));
	connect(discover, SIGNAL(recordLost(Record)), this, SLOT(recordLost(Record)));
	discover->addRecord(record);
	discover->addGlobalServer("76.14.48.127");
	discover->start();
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
