

#include "DroneFind.h"
#include <QDebug>
#include <QCoreApplication>
#include <iostream>

void ignoreMessages (QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
     Q_UNUSED(type);
     Q_UNUSED(context);
     Q_UNUSED(msg);
}

DroneFind::DroneFind (QObject *parent) : QObject(parent)
{
	// Initialize
	mDone = false;
	
	// TODO Parse arguments
	QStringList args = QCoreApplication::arguments();
	if (args.size() < 3) QCoreApplication::exit(1);
	QString key = args[1];
	QString value = args[2];
	
	// Disable Qt debug messages
	qInstallMessageHandler(ignoreMessages);
	
	// Start peer discovery
	Record record;

	mDiscover = new Discover(this);
	connect(mDiscover, SIGNAL(recordFound(Record)), this, SLOT(recordFound(Record)));
	connect(mDiscover, SIGNAL(recordLost(Record)), this, SLOT(recordLost(Record)));
	mDiscover->addRecord(record);
	mDiscover->addFilter(key, value);
	mDiscover->start();
}

void DroneFind::recordFound (Record record)
{
	if (not mDone)
	{
		mDone = true;
		std::cout << record["Address"].toStdString() << std::endl;
		QCoreApplication::exit();
	}
}

void DroneFind::recordLost (Record record)
{
     Q_UNUSED(record);
}
