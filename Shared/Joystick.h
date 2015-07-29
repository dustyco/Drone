#ifndef JOYSTICK_H
#define JOYSTICK_H

#include <QObject>
#include <QList>
#include <QPair>

// Forward declaration
class JoystickImpl;

class Joystick : public QObject
{
	Q_OBJECT

public:
	struct State
	{
		State ()
		{
			joystick = -1;
			isConnected = false;
		}

		int joystick;
		bool isConnected;
		QString api, name;
		QList<float> axes;
		QList<bool> buttons;
		QList<QPair<int,bool> > buttonEvents;
	};

	explicit Joystick(QObject *parent = 0);
	State getState (int joystick);

signals:
	void connected (int joystick);
	void disconnected (int joystick);
	void buttonAction (int joystick, int button, bool isPressed);
	void axisAction (int joystick);

public slots:
	void update ();

private:
	JoystickImpl* impl;
};

#endif // JOYSTICK_H
