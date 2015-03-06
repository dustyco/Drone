#ifndef DRONEFIND_H
#define DRONEFIND_H

#include <QObject>
#include <QTimer>
#include "../Shared/Discover.h"

class DroneFind : public QObject
{
    Q_OBJECT

public:
	explicit DroneFind (QObject *parent = 0);

signals:

public slots:
	void recordFound (Record record);
	void recordLost (Record record);

private:
    Discover* mDiscover;
    QTimer mTimer;
    bool mDone;
};

#endif // DRONEFIND_H
