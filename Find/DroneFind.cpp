

#include "DroneFind.h"
#include "../Shared/Config.h"
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

	// Load the configuration
	Config& config = Config::getSingleton();
	config.prefix = "Find/";
	QString configError = checkConfig();
	if (not configError.isEmpty())
	{
		qDebug() << "Config error:" << configError;
	}
	QHostAddress towerAddress(config.value("Tower Address").toString());
	quint16 towerPort = config.value("Tower Port").toInt();

	QHostAddress multicastAddress(config.value("Multicast Address").toString());
	quint16 multicastPort = config.value("Multicast Port").toInt();
	
	// Disable Qt debug messages
	//qInstallMessageHandler(ignoreMessages);
	
	// Start peer discovery
	mDiscover = new Discover(this, Discover::NormalMode, multicastAddress, multicastPort);
	connect(mDiscover, SIGNAL(recordFound(Record)), this, SLOT(recordFound(Record)));
	connect(mDiscover, SIGNAL(recordLost(Record)), this, SLOT(recordLost(Record)));
	mDiscover->addFilter(key, value);
	mDiscover->addGlobalServer(towerAddress, towerPort);
	mDiscover->setDefaultScope("Global");
	mDiscover->start();
}

QString DroneFind::checkConfig()
{
	Config& config = Config::getSingleton();
	QString error("Error in DroneConfig.ini [Find]: ");

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
