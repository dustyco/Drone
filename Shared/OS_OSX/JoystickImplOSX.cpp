#include "JoystickImplOSX.h"
#include <IOKit/IOKitLib.h>

JoystickImplOSX::JoystickImplOSX(QObject *parent) : JoystickImpl(parent)
{
	setLoggerTag("JoystickImplOSX");
	logInfo("Constructed");
//	CFDictionaryRef matching;
//	io_iterator_t* existing;
//	IOServiceGetMatchingServices(kIOMasterPortDefault, myMatchingDictionary, &myIterator);
}

