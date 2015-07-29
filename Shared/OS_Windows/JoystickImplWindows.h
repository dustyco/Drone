#ifndef JOYSTICKIMPLWINDOWS_H
#define JOYSTICKIMPLWINDOWS_H

#include <QObject>
#include "../JoystickImpl.h"

class JoystickImplWindows : public JoystickImpl
{
public:
        explicit JoystickImplWindows(QObject *parent = 0);

signals:

public slots:
};

#endif // JOYSTICKIMPLWINDOWS_H
