#include "Joystick.h"
#include "JoystickImpl.h"

#if defined(OS_WINDOWS)
	#include "OS_Windows/JoystickImplWindows.h"
#elif defined(OS_OSX)
	#include "OS_OSX/JoystickImplOSX.h"
#elif defined(OS_LINUX)
	#include "OS_Linux/JoystickImplLinux.h"
#endif

Joystick::Joystick(QObject *parent) : QObject(parent)
{
#if defined(OS_WINDOWS)
	impl = new JoystickImplWindows(this);

#elif defined(OS_OSX)
	impl = new JoystickImplOSX(this);

#elif defined(OS_LINUX)
	impl = new JoystickImplLinux(this);
#else
	impl = new JoystickImpl(this);

#endif
}

Joystick::State Joystick::getState(int joystick)
{

}

void Joystick::update()
{

}

