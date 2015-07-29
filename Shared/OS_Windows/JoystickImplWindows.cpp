#include "JoystickImplWindows.h"

JoystickImplWindows::JoystickImplWindows(QObject *parent) : JoystickImpl(parent)
{
	setLoggerTag("JoystickImplWindows");
	logInfo("Constructed");
}

