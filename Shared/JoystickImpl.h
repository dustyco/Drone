#ifndef JOYSTICKIMPL_H
#define JOYSTICKIMPL_H

#include <QObject>
#include "Logger.h"

class JoystickImpl : public QObject, public Logger
{
	Q_OBJECT
	
public:
	explicit JoystickImpl(QObject *parent = 0);

signals:

public slots:
};

#endif // JOYSTICKIMPL_H
