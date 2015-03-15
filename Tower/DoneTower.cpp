

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
	//quint16 port = config.value("Port").toInt();

	// Start Discovery
	Record record;
	record["Service"] = "DroneTower";
	record["Scope"] = "Global";

	mDiscover = new Discover(this, true);
	mDiscover->addRecord(record);
	mDiscover->start();

}
