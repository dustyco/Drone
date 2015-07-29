#ifndef JOYSTICKIMPLOSX_H
#define JOYSTICKIMPLOSX_H

#include <QObject>
#include "../JoystickImpl.h"

class JoystickImplOSX : public JoystickImpl
{
public:
        explicit JoystickImplOSX(QObject *parent = 0);

signals:

public slots:
};

#endif // JOYSTICKIMPLOSX_H
