

#include "DroneTower.h"
#include "../Shared/Config.h"



DroneTower::DroneTower (QObject *parent) : QObject(parent)
{
		// Load the configuration
		Config& config = Config::getSingleton();
		config.prefix = "Tower/";
		//quint16 port = config.value("Port").toInt();

		// Start Discovery
		Record record;
		record["Service"] = "DroneTower";
		record["Scope"] = "Global";

		mDiscover = new Discover(this);
		mDiscover->setGlobalServerMode(true);
		mDiscover->addRecord(record);
		mDiscover->start();

}
