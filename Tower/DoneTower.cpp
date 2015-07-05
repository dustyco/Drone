

#include "DroneTower.h"
#include "../Shared/Config.h"
#include <iostream>


DroneTower::DroneTower (QObject *parent) : QObject(parent)
{
	// Print the version
	QString version(GIT_VERSION);
	std::cout << "DroneTower " << version.toStdString() << std::endl;

	// Load the configuration
	Config& config = Config::getSingleton();
	config.prefix = "Tower/";
	QString configError = checkConfig();
	if (not configError.isEmpty())
	{
		qDebug() << "Config error:" << configError;
	}
	quint16 globalPort = config.value("Global Port").toInt();

	// Start Discovery
	Record record;
	record["Service"] = "DroneTower";
	record["Scope"] = "Global";

	mDiscover = new Discover(this, Discover::ServerMode, QHostAddress(), globalPort);
	mDiscover->addRecord(record);
	mDiscover->start();

}

QString DroneTower::checkConfig()
{
	Config& config = Config::getSingleton();
	QString error("Error in DroneConfig.ini [Tower]: ");

	// TODO Read the file ok

	quint16 multicastPort = config.value("Global Port").toInt();
	if (multicastPort < 1) return error + "Global Port: " + config.value("Global Port").toString();

	return "";
}
