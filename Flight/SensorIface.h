#ifndef SENSORIFACE_H
#define SENSORIFACE_H

#include <QObject>
#include <QSerialPortInfo>
#include <QTimer>
#include "../Shared/Discover.h"
#include "../Shared/Logger.h"

class SensorIface : public QObject, public Logger
{
	Q_OBJECT

public:
	enum Mode { TTYMode, DiscoverMode, ErrorMode};

public:
	explicit SensorIface (QObject* parent = 0, Discover* discover = 0);
	bool start ();

signals:

private slots:
	void ttyQuery ();
	bool ttyMatch (const QSerialPortInfo& serialPortInfo, QString description);

private:
	Mode mMode;
	QTimer* mTTYTimer;
	Discover* mDiscover;
};

#endif // SENSORIFACE_H
