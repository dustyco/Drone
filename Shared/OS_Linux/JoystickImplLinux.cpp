
#include "JoystickImplLinux.h"

JoystickImplLinux::JoystickImplLinux(QObject *parent) : JoystickImpl(parent)
{
	setLoggerTag("JoystickImplLinux");
	logInfo("Constructed");
	udevContext = 0;
	udevMonitor = 0;
}

bool JoystickImplLinux::initialize ()
{
    udevContext = udev_new();
    if (!udevContext)
    {
		logWarning("udev_new() failed");
        return false;
    }
    
    udevMonitor = udev_monitor_new_from_netlink(udevContext, "udev");
    if (!udevMonitor)
    {
		logWarning("udev_monitor_new_from_netlink() failed");
        return false;
    }
    
    int error = udev_monitor_filter_add_match_subsystem_devtype(udevMonitor, "input", NULL);
    if (error < 0)
    {
		logWarning(QString("udev_monitor_filter_add_match_subsystem_devtype() failed: %1").arg(error));

        udev_monitor_unref(udevMonitor);
        udevMonitor = 0;
        return false;
    }
    
    error = udev_monitor_enable_receiving(udevMonitor);
    if (error < 0)
    {
		logWarning(QString("udev_monitor_enable_receiving() failed: %1").arg(error));

        udev_monitor_unref(udevMonitor);
        udevMonitor = 0;
        return false;
   }
}
