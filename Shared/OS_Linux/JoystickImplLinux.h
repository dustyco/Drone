#ifndef JOYSTICKIMPLLINUX_H
#define JOYSTICKIMPLLINUX_H

#include <QObject>
#include "../JoystickImpl.h"
#include <linux/joystick.h>
#include <libudev.h>

class JoystickImplLinux : public JoystickImpl
{
public:
        explicit JoystickImplLinux(QObject *parent = 0);
        bool initialize ();

signals:

public slots:

private:
    udev* udevContext;
    udev_monitor* udevMonitor;

    struct JoystickRecord
    {
        std::string deviceNode;
        std::string systemPath;
        bool plugged;
    };

    typedef std::vector<JoystickRecord> JoystickList;
    JoystickList joystickList;
};

#endif // JOYSTICKIMPLLINUX_H
